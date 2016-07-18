#include <Editor/ModelWidget.hpp>
#include <Nazara/Core.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <Ndk/Application.hpp>
#include <Ndk/Components.hpp>
#include <Ndk/Systems.hpp>
#include <iostream>

ModelWidget::ModelWidget(QWidget* parent) :
QtCanvas(parent),
m_cameraAngles(-30, 45, 0),
m_cameraDistance(5)
{
	m_updateTimer.setInterval(0);
	m_updateTimer.connect(&m_updateTimer, &QTimer::timeout, this, &ModelWidget::Update);
	m_updateTimer.start();

	m_normalMaterial = Nz::Material::New();
	m_normalMaterial->SetDiffuseColor(Nz::Color::Red);
	m_normalMaterial->SetShader("Basic");

	m_normalModel = Nz::Model::New();

	m_disabledModel = Nz::Model::New();
	m_disabledMaterial = Nz::Material::New();
	m_disabledMaterial->Enable(Nz::RendererParameter_Blend, true);
	m_disabledMaterial->Enable(Nz::RendererParameter_DepthWrite, false);
	m_disabledMaterial->Enable(Nz::RendererParameter_FaceCulling, false);
	m_disabledMaterial->SetDiffuseColor(Nz::Color(128, 128, 128, 50));
	m_disabledMaterial->SetDstBlend(Nz::BlendFunc_InvSrcAlpha);
	m_disabledMaterial->SetSrcBlend(Nz::BlendFunc_SrcAlpha);

	m_camera = m_world.CreateEntity();
	m_camera->AddComponent<Ndk::NodeComponent>();

	m_gridModel = Nz::Model::New();
	m_gridModel->SetMesh(CreateGridMesh(31));

	m_grid = m_world.CreateEntity();
	m_grid->AddComponent<Ndk::NodeComponent>();
	m_grid->AddComponent<Ndk::GraphicsComponent>().Attach(m_gridModel);

	m_modelEntity = m_world.CreateEntity();
	m_modelEntity->AddComponent<Ndk::NodeComponent>().SetParent(m_grid);
	m_modelEntity->AddComponent<Ndk::GraphicsComponent>();

	Ndk::CameraComponent& viewer = m_camera->AddComponent<Ndk::CameraComponent>();
	viewer.SetTarget(this);
	viewer.SetZFar(5000.f);

	Ndk::EntityHandle light = m_world.CreateEntity();
	Ndk::NodeComponent& lightNode = light->AddComponent<Ndk::NodeComponent>();
	Ndk::LightComponent& lightComp = light->AddComponent<Ndk::LightComponent>(Nz::LightType_Directional);
	lightNode.SetRotation(Nz::EulerAnglesf(-30.f, 102.f, 0.f));

	UpdateCamera();
}

ModelWidget::~ModelWidget()
{
}

void ModelWidget::InvalidateNormals()
{
	m_normalModel->SetMesh(nullptr);
}

void ModelWidget::OnModelChanged(const Nz::ModelRef& model)
{
	Ndk::GraphicsComponent& modelGraphics = m_modelEntity->GetComponent<Ndk::GraphicsComponent>();
	modelGraphics.Clear();

	m_model = model;

	if (m_model)
	{
		Nz::Mesh* mesh = m_model->GetMesh();
		m_disabledModel->SetMesh(mesh);
		for (std::size_t i = 0; i < mesh->GetMaterialCount(); ++i)
			m_disabledModel->SetMaterial(i, m_disabledMaterial);

		m_normalModel->SetMesh(nullptr);

		m_box = model->GetBoundingVolume().obb.localBox;

		modelGraphics.Attach(model);
		modelGraphics.Attach(m_disabledModel);

		Nz::Vector3f center = m_box.GetCenter();

		Ndk::NodeComponent& modelNode = m_modelEntity->GetComponent<Ndk::NodeComponent>();
		modelNode.SetPosition(-center.x, m_box.GetLengths().y / 2.f - center.y, -center.z);
		ResetCamera();
	}
	else
		m_box.MakeZero();
}

void ModelWidget::ResetCamera()
{
	m_cameraAngles.MakeZero();
	m_cameraDistance = m_box.GetRadius();

	UpdateCamera();
}

void ModelWidget::ShowNormals(bool normals)
{
	if (!m_normalModel->GetMesh() && normals)
	{
		Nz::Mesh* mesh = m_model->GetMesh();

		Nz::MeshRef normalMesh = Nz::Mesh::New();
		normalMesh->CreateStatic();

		for (std::size_t i = 0; i < mesh->GetSubMeshCount(); ++i)
		{
			Nz::StaticMesh* submesh = static_cast<Nz::StaticMesh*>(mesh->GetSubMesh(i));
			unsigned int vertexCount = submesh->GetVertexCount();

			Nz::VertexBufferRef vertexBuffer = Nz::VertexBuffer::New(Nz::VertexDeclaration::Get(Nz::VertexLayout_XYZ), vertexCount * 2, Nz::DataStorage_Hardware);
			Nz::VertexMapper originalMapper(submesh);
			Nz::VertexMapper normalMapper(vertexBuffer);

			Nz::SparsePtr<Nz::Vector3f> oriPtr = originalMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent_Position);
			Nz::SparsePtr<Nz::Vector3f> normalPtr = originalMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent_Normal);
			Nz::SparsePtr<Nz::Vector3f> linePtr = normalMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent_Position);

			for (unsigned int j = 0; j < vertexCount; ++j)
			{
				Nz::Vector3f position = *oriPtr++;
				Nz::Vector3f normal = *normalPtr++;

				*linePtr++ = position;
				*linePtr++ = position + normal / 5.f;
			}

			originalMapper.Unmap();
			normalMapper.Unmap();

			Nz::StaticMeshRef normalSubmesh = Nz::StaticMesh::New(normalMesh);
			normalSubmesh->Create(vertexBuffer);
			normalSubmesh->GenerateAABB();
			normalSubmesh->SetPrimitiveMode(Nz::PrimitiveMode_LineList);
			normalSubmesh->SetMaterialIndex(0);

			normalMesh->AddSubMesh(normalSubmesh);
		}

		m_normalModel->SetMesh(normalMesh);
		m_normalModel->SetMaterial(0, m_normalMaterial);
	}

	if (normals)
		m_modelEntity->GetComponent<Ndk::GraphicsComponent>().Attach(m_normalModel);
	else
		m_modelEntity->GetComponent<Ndk::GraphicsComponent>().Detach(m_normalModel);
}

void ModelWidget::ShowSubmeshes(const Nz::Bitset<>& submeshes)
{
	m_disabledModel->ShowSubmeshes(~submeshes);
	m_model->ShowSubmeshes(submeshes);
	m_normalModel->ShowSubmeshes(submeshes);
}

Nz::MeshRef ModelWidget::CreateGridMesh(unsigned int size)
{
	Nz::VertexDeclaration* declaration = Nz::VertexDeclaration::Get(Nz::VertexLayout_XYZ);

	unsigned int indexCount = size * 2 * 2;
	unsigned int vertexCount = size * 2 * 2;

	Nz::IndexBufferRef indexBuffer   = Nz::IndexBuffer::New(vertexCount > std::numeric_limits<Nz::UInt16>::max(), indexCount, Nz::DataStorage_Hardware, Nz::BufferUsage_Static);
	Nz::VertexBufferRef vertexBuffer = Nz::VertexBuffer::New(declaration, vertexCount, Nz::DataStorage_Hardware, Nz::BufferUsage_Static);

	Nz::VertexMapper vertexMapper(vertexBuffer, Nz::BufferAccess_WriteOnly);

	Nz::SparsePtr<Nz::Vector3f> positionPtr = vertexMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent_Position);
	
	float halfSize = float(size) / 2.f;

	float pos = -halfSize + 0.5f;
	for (unsigned int i = 0; i < size; ++i)
	{
		*positionPtr++ = Nz::Vector3f(pos, 0.f, -halfSize);
		*positionPtr++ = Nz::Vector3f(pos, 0.f, halfSize);
		pos += 1.f;
	}

	pos = -halfSize + 0.5f;
	for (unsigned int i = 0; i < size; ++i)
	{
		*positionPtr++ = Nz::Vector3f(-halfSize, 0.f, pos);
		*positionPtr++ = Nz::Vector3f(halfSize, 0.f,  pos);
		pos += 1.f;
	}

	vertexMapper.Unmap();

	Nz::IndexMapper indexMapper(indexBuffer, Nz::BufferAccess_WriteOnly);

	for (unsigned int i = 0; i < indexCount; ++i)
		indexMapper.Set(i, i);

	indexMapper.Unmap();

	Nz::MeshRef gridMesh = Nz::Mesh::New();
	gridMesh->CreateStatic();

	Nz::StaticMeshRef subMesh = Nz::StaticMesh::New(gridMesh);
	if (!subMesh->Create(vertexBuffer))
	{
		NazaraError("Failed to create StaticMesh");
		return nullptr;
	}

	subMesh->GenerateAABB();
	subMesh->SetIndexBuffer(indexBuffer);
	subMesh->SetMaterialIndex(0);
	subMesh->SetPrimitiveMode(Nz::PrimitiveMode_LineList);

	gridMesh->AddSubMesh(subMesh);
	gridMesh->SetMaterialCount(1);

	return gridMesh;
}

void ModelWidget::HandleEvent(const Nz::WindowEvent& event)
{
	switch (event.type)
	{
		case Nz::WindowEventType_MouseWheelMoved:
		{
			m_cameraDistance -= event.mouseWheel.delta * m_cameraDistance / 100.f;

			UpdateCamera();
			break;
		}

		case Nz::WindowEventType_MouseButtonPressed:
		{
			switch (event.mouseButton.button)
			{
				case Nz::Mouse::Left:
					SetCursor(Nz::WindowCursor_None);
					m_mouseControl = MouseControl::Rotation;
					break;

				case Nz::Mouse::Right:
					SetCursor(Nz::WindowCursor_None);
					m_mouseControl = MouseControl::Movement;
					break;
			}
			break;
		}

		case Nz::WindowEventType_MouseButtonReleased:
		{
			switch (event.mouseButton.button)
			{
				case Nz::Mouse::Left:
				case Nz::Mouse::Right:
					m_mouseControl = MouseControl::None;
					SetCursor(Nz::WindowCursor_Default);
					break;
			}
			break;
		}

		case Nz::WindowEventType_MouseMoved:
		{
			switch (m_mouseControl)
			{
				case MouseControl::Movement:
				{
					constexpr float speed = 0.1f;

					Ndk::NodeComponent& cameraNode = m_camera->GetComponent<Ndk::NodeComponent>();
					Ndk::NodeComponent& modelNode = m_modelEntity->GetComponent<Ndk::NodeComponent>();

					Nz::Vector3f relativeDirection = cameraNode.GetRight() * event.mouseMove.deltaX + cameraNode.GetDown() * event.mouseMove.deltaY;
					relativeDirection.Normalize();

					if (!Nz::Keyboard::IsKeyPressed(Nz::Keyboard::LShift) && !Nz::Keyboard::IsKeyPressed(Nz::Keyboard::RShift))
						modelNode.Move(relativeDirection * speed);
					else
					{
						std::array<Nz::Vector3f, 6> directions = {
							Nz::Vector3f::Backward(),
							Nz::Vector3f::Down(),
							Nz::Vector3f::Forward(),
							Nz::Vector3f::Left(),
							Nz::Vector3f::Right(),
							Nz::Vector3f::Up()
						};

						Nz::Vector3f axisDirection = directions[0];
						float axisScore = axisDirection.DotProduct(relativeDirection);
						for (auto it = directions.begin()+1; it != directions.end(); ++it)
						{
							float score = it->DotProduct(relativeDirection);
							if (score > axisScore)
							{
								axisDirection = *it;
								axisScore = score;
							}
						}

						modelNode.Move(axisDirection * speed);
					}
					break;
				}

				case MouseControl::Rotation:
				{
					constexpr float sensitivity = 0.3f;

					m_cameraAngles.yaw = Nz::NormalizeAngle(m_cameraAngles.yaw - event.mouseMove.deltaX*sensitivity);
					m_cameraAngles.pitch = Nz::Clamp(m_cameraAngles.pitch - event.mouseMove.deltaY*sensitivity, -89.f, 89.f);

					UpdateCamera();
					break;
				}

				case MouseControl::None:
				default:
					break;
			}
			break;
		}
	}
}

void ModelWidget::UpdateCamera()
{
	Ndk::NodeComponent& cameraNode = m_camera->GetComponent<Ndk::NodeComponent>();
	cameraNode.SetRotation(m_cameraAngles);
	cameraNode.SetPosition(cameraNode.GetBackward() * m_cameraDistance);
}

bool ModelWidget::OnWindowCreated()
{
	QtCanvas::OnWindowCreated();

	SetCursor(Nz::WindowCursor_Default);
	SetEventListener(true);

	return true;
}

void ModelWidget::OnWindowResized()
{
	QtCanvas::OnWindowResized();
}

void ModelWidget::Update()
{
	float elapsedTime = m_updateClock.GetSeconds();
	m_updateClock.Restart();

	m_world.Update(elapsedTime);

	Nz::DebugDrawer::DrawAxes();

	Nz::WindowEvent event;
	while (PollEvent(&event))
		HandleEvent(event);

	Display();
}

#include <Editor/ModelWidget_moc.inl>
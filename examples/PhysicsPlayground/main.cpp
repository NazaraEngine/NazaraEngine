#include <Nazara/Core.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Platform/WindowingAppComponent.hpp>
#include <Nazara/Physics3D.hpp>
#include <Nazara/Renderer.hpp>
#include <iostream>
#include <random>

constexpr float BoxDims = 16.f;

int main(int argc, char* argv[])
{
	try {
	// Mise en place de l'application, de la fenêtre et du monde
	Nz::Renderer::Config renderConfig;
	renderConfig.validationLevel = Nz::RenderAPIValidationLevel::None;

	Nz::Application<Nz::Graphics, Nz::Physics3D> app(argc, argv, renderConfig);

	auto& windowing = app.AddComponent<Nz::WindowingAppComponent>();
	Nz::Window& mainWindow = windowing.CreateWindow(Nz::VideoMode(1280, 720), "Physics playground");

	auto& fs = app.AddComponent<Nz::FilesystemAppComponent>();
	{
		std::filesystem::path resourceDir = "assets/examples";
		if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
			resourceDir = "../.." / resourceDir;

		fs.Mount("assets", resourceDir);
	}

	auto& ecs = app.AddComponent<Nz::EntitySystemAppComponent>();
	auto& world = ecs.AddWorld<Nz::EnttWorld>();

	auto& physSystem = world.AddSystem<Nz::Physics3DSystem>();
	physSystem.GetPhysWorld().SetMaxStepCount(1);
	physSystem.GetPhysWorld().SetStepSize(Nz::Time::TickDuration(30));
	physSystem.GetPhysWorld().SetGravity(Nz::Vector3f::Down() * 9.81f);
	//physSystem.GetPhysWorld().SetGravity(Nz::Vector3f::Zero());

	auto& renderSystem = world.AddSystem<Nz::RenderSystem>();
	Nz::WindowSwapchain& windowSwapchain = renderSystem.CreateSwapchain(mainWindow);

	Nz::Vector3f target = Nz::Vector3f::Zero();

	std::shared_ptr<Nz::MaterialInstance> colliderMat = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Basic);
	colliderMat->SetValueProperty("BaseColor", Nz::Color::sRGBToLinear(Nz::Color::Green()));
	colliderMat->UpdatePassesStates([](Nz::RenderStates& states)
	{
		states.primitiveMode = Nz::PrimitiveMode::LineList;
		return true;
	});

	entt::handle boxColliderEntity = world.CreateEntity();
	{
		std::shared_ptr<Nz::GraphicalMesh> boxMesh = Nz::GraphicalMesh::Build(Nz::Primitive::Box(Nz::Vector3f(BoxDims), Nz::Vector3ui::Zero(), Nz::Matrix4f::Scale(Nz::Vector3f(-1.f)), Nz::Rectf(0.f, 0.f, 2.f, 2.f)));

		Nz::TextureSamplerInfo planeSampler;
		planeSampler.anisotropyLevel = 16;
		planeSampler.wrapModeU = Nz::SamplerWrap::Repeat;
		planeSampler.wrapModeV = Nz::SamplerWrap::Repeat;

		std::shared_ptr<Nz::MaterialInstance> boxMat = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Phong);
		boxMat->SetTextureProperty("BaseColorMap", fs.Open<Nz::TextureAsset>("assets/dev_grey.png"), planeSampler);
		boxMat->DisablePass("ShadowPass");
		boxMat->UpdatePassesStates([&](Nz::RenderStates& states)
		{
			states.frontFace = Nz::FrontFace::Clockwise;
		});

		std::shared_ptr<Nz::Model> boxModel = std::make_shared<Nz::Model>(std::move(boxMesh));
		boxModel->SetMaterial(0, std::move(boxMat));

		auto& boxGfx = boxColliderEntity.emplace<Nz::GraphicsComponent>();
		boxGfx.AttachRenderable(std::move(boxModel));

		boxColliderEntity.emplace<Nz::NodeComponent>();

		float thickness = 1.f;
		std::shared_ptr<Nz::BoxCollider3D> wallCollider = std::make_shared<Nz::BoxCollider3D>(Nz::Vector3f(BoxDims + thickness * 2.f, BoxDims + thickness * 2.f, thickness));

		std::vector<Nz::CompoundCollider3D::ChildCollider> colliders;
		for (Nz::Vector3f normal : { Nz::Vector3f::Forward(), Nz::Vector3f::Backward(), Nz::Vector3f::Left(), Nz::Vector3f::Right(), Nz::Vector3f::Up(), Nz::Vector3f::Down() })
		{
			auto& colliderEntry = colliders.emplace_back();
			colliderEntry.collider = wallCollider;
			colliderEntry.rotation = Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), normal);
			colliderEntry.offset = normal * BoxDims * 0.5f + normal * 0.5f;
		}

		std::shared_ptr<Nz::CompoundCollider3D> boxCollider = std::make_shared<Nz::CompoundCollider3D>(std::move(colliders));

		Nz::RigidBody3D::StaticSettings settings;
		settings.geom = boxCollider;

		boxColliderEntity.emplace<Nz::RigidBody3DComponent>(settings);

		std::shared_ptr<Nz::Model> colliderModel;
		{
			std::shared_ptr<Nz::Mesh> colliderMesh = Nz::Mesh::Build(boxCollider->GenerateDebugMesh());
			std::shared_ptr<Nz::GraphicalMesh> colliderGraphicalMesh = Nz::GraphicalMesh::BuildFromMesh(*colliderMesh);

			colliderModel = std::make_shared<Nz::Model>(colliderGraphicalMesh);
			for (std::size_t i = 0; i < colliderModel->GetSubMeshCount(); ++i)
				colliderModel->SetMaterial(i, colliderMat);

			boxGfx.AttachRenderable(std::move(colliderModel));
		}
	}

	std::shared_ptr<Nz::GraphicalMesh> sphereMesh = Nz::GraphicalMesh::Build(Nz::Primitive::IcoSphere(1.f));

	//std::mt19937 rd(std::random_device{}());
	std::mt19937 rd(42);
	std::uniform_real_distribution<float> colorDis(0.f, 360.f);
	std::uniform_real_distribution<float> radiusDis(0.1f, 0.5f);

	constexpr std::size_t SphereCount = 1000;
	for (std::size_t i = 0; i < SphereCount; ++i)
	{
		float radius = radiusDis(rd);
		std::uniform_real_distribution<float> positionRandom(-BoxDims * 0.5f + radius, BoxDims * 0.5f - radius);

		std::shared_ptr<Nz::SphereCollider3D> sphereCollider = std::make_shared<Nz::SphereCollider3D>(radius);

		entt::handle ballEntity = world.CreateEntity();

		std::shared_ptr<Nz::MaterialInstance> ballMaterial = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Phong);
		ballMaterial->SetValueProperty("BaseColor", Nz::Color::sRGBToLinear(Nz::Color::FromHSV(colorDis(rd), 1.f, 1.f)));

		std::shared_ptr<Nz::Model> sphereModel = std::make_shared<Nz::Model>(sphereMesh);
		sphereModel->SetMaterial(0, std::move(ballMaterial));

		auto& ballGfx = ballEntity.emplace<Nz::GraphicsComponent>();
		ballGfx.AttachRenderable(std::move(sphereModel));

		auto& ballNode = ballEntity.emplace<Nz::NodeComponent>();
		ballNode.SetPosition({ positionRandom(rd), positionRandom(rd), positionRandom(rd) });
		ballNode.SetScale(radius);

		Nz::RigidBody3D::DynamicSettings settings;
		settings.geom = sphereCollider;
		settings.mass = 4.f / 3.f * Nz::Pi<float> * Nz::IntegralPow(radius, 3);

		ballEntity.emplace<Nz::RigidBody3DComponent>(settings);
	}

	std::uniform_real_distribution<float> lengthDis(0.2f, 1.5f);
	std::shared_ptr<Nz::GraphicalMesh> boxMesh = Nz::GraphicalMesh::Build(Nz::Primitive::Box(Nz::Vector3f(1.f)));

	constexpr std::size_t BoxCount = 100;
	for (std::size_t i = 0; i < BoxCount; ++i)
	{
		float width = lengthDis(rd);
		float height = lengthDis(rd);
		float depth = lengthDis(rd);

		std::uniform_real_distribution<float> xRandom(-BoxDims * 0.5f + width, BoxDims * 0.5f - width);
		std::uniform_real_distribution<float> yRandom(-BoxDims * 0.5f + height, BoxDims * 0.5f - height);
		std::uniform_real_distribution<float> zRandom(-BoxDims * 0.5f + depth, BoxDims * 0.5f - depth);

		entt::handle boxEntity = world.CreateEntity();

		std::shared_ptr<Nz::MaterialInstance> boxMaterial = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Phong);
		boxMaterial->SetValueProperty("BaseColor", Nz::Color::sRGBToLinear(Nz::Color::FromHSV(colorDis(rd), 1.f, 1.f)));

		std::shared_ptr<Nz::Model> sphereModel = std::make_shared<Nz::Model>(boxMesh);
		sphereModel->SetMaterial(0, std::move(boxMaterial));

		boxEntity.emplace<Nz::GraphicsComponent>(std::move(sphereModel));

		auto& ballNode = boxEntity.emplace<Nz::NodeComponent>();
		ballNode.SetPosition({ xRandom(rd), yRandom(rd), zRandom(rd) });
		ballNode.SetScale({ width, height, depth });

		std::shared_ptr<Nz::BoxCollider3D> boxCollider = std::make_shared<Nz::BoxCollider3D>(Nz::Vector3f(width, height, depth));

		Nz::RigidBody3D::DynamicSettings settings;
		settings.geom = boxCollider;
		settings.mass = width * height * depth;

		boxEntity.emplace<Nz::RigidBody3DComponent>(settings);
	}

	// Spaceships
	{
		Nz::MeshParams meshParams;
		meshParams.center = true;
		meshParams.vertexRotation = Nz::EulerAnglesf(0.f, 90.f, 0.f);
		meshParams.vertexScale = Nz::Vector3f(0.002f);
		meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV_Tangent);

		std::shared_ptr<Nz::Mesh> spaceshipMesh = fs.Load<Nz::Mesh>("assets/Spaceship/spaceship.obj", meshParams);
		if (!spaceshipMesh)
		{
			NazaraError("failed to load model");
			return __LINE__;
		}

		const Nz::Boxf& aabb = spaceshipMesh->GetAABB();

		std::uniform_real_distribution<float> xRandom(-BoxDims * 0.5f + aabb.width, BoxDims * 0.5f - aabb.width);
		std::uniform_real_distribution<float> yRandom(-BoxDims * 0.5f + aabb.height, BoxDims * 0.5f - aabb.height);
		std::uniform_real_distribution<float> zRandom(-BoxDims * 0.5f + aabb.depth, BoxDims * 0.5f - aabb.depth);


		std::shared_ptr<Nz::GraphicalMesh> gfxMesh = Nz::GraphicalMesh::BuildFromMesh(*spaceshipMesh);

		Nz::TextureSamplerInfo samplerInfo;
		samplerInfo.anisotropyLevel = 8;

		std::shared_ptr<Nz::MaterialInstance> material = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Phong);

		material->SetTextureProperty("BaseColorMap", fs.Open<Nz::TextureAsset>("assets/Spaceship/Texture/diffuse.png"));

		std::shared_ptr<Nz::Model> model = std::make_shared<Nz::Model>(std::move(gfxMesh));
		for (std::size_t i = 0; i < model->GetSubMeshCount(); ++i)
			model->SetMaterial(i, material);

		Nz::VertexMapper vertexMapper(*spaceshipMesh->GetSubMesh(0));
		Nz::SparsePtr<Nz::Vector3f> vertices = vertexMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent::Position);

		auto shipCollider = std::make_shared<Nz::ConvexHullCollider3D>(vertices, vertexMapper.GetVertexCount(), 0.1f);

		std::shared_ptr<Nz::Model> colliderModel;
		{
			std::shared_ptr<Nz::Mesh> colliderMesh = Nz::Mesh::Build(shipCollider->GenerateDebugMesh());
			std::shared_ptr<Nz::GraphicalMesh> colliderGraphicalMesh = Nz::GraphicalMesh::BuildFromMesh(*colliderMesh);

			colliderModel = std::make_shared<Nz::Model>(colliderGraphicalMesh);
			for (std::size_t i = 0; i < colliderModel->GetSubMeshCount(); ++i)
				colliderModel->SetMaterial(i, colliderMat);
		}

		constexpr std::size_t ShipCount = 20;
		for (std::size_t i = 0; i < ShipCount; ++i)
		{
			entt::handle shipEntity = world.CreateEntity();

			shipEntity.emplace<Nz::GraphicsComponent>(model);

			auto& shipNode = shipEntity.emplace<Nz::NodeComponent>();
			shipNode.SetPosition({ xRandom(rd), yRandom(rd), zRandom(rd) });

			Nz::RigidBody3D::DynamicSettings settings;
			settings.geom = shipCollider;
			settings.mass = 100.f;

			shipEntity.emplace<Nz::RigidBody3DComponent>(settings);

			//shipEntity.get<Nz::GraphicsComponent>().AttachRenderable(colliderModel);
		}
	}

	// Lumière
	entt::handle lightEntity = world.CreateEntity();
	{
		auto& lightNode = lightEntity.emplace<Nz::NodeComponent>();
		//lightNode.SetPosition(Nz::Vector3f(-20.f, 20.f, -20.f));
		//lightNode.SetRotation(Nz::EulerAnglesf(-45.f, -135.f, 0.f));
		lightNode.SetPosition(Nz::Vector3f::Up() * 15.f);
		lightNode.SetRotation(Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), Nz::Vector3f::Down()));

		auto& lightComp = lightEntity.emplace<Nz::LightComponent>();
		auto& spotLight = lightComp.AddLight<Nz::SpotLight>();
		spotLight.EnableShadowCasting(true);
		spotLight.UpdateInnerAngle(Nz::DegreeAnglef(30.f));
		spotLight.UpdateOuterAngle(Nz::DegreeAnglef(40.f));
		spotLight.UpdateRadius(30.f);
		spotLight.UpdateShadowMapSize(2048);
		spotLight.UpdateAmbientFactor(0.5f);
	}

	Nz::EulerAnglesf camAngles(-45.f, -135.f, 0.f);
	float camDistance = 12.f;

	// Création de la caméra
	entt::handle cameraEntity = world.CreateEntity();
	{
		cameraEntity.emplace<Nz::NodeComponent>();

		auto& cameraComponent = cameraEntity.emplace<Nz::CameraComponent>(std::make_shared<Nz::RenderWindow>(windowSwapchain), Nz::ProjectionType::Perspective);
		cameraComponent.UpdateFOV(70.f);
		cameraComponent.UpdateClearColor(Nz::Color::sRGBToLinear(Nz::Color(0.46f, 0.48f, 0.84f, 1.f)));
	}

	auto UpdateCamera = [&]
	{
		auto& cameraNode = cameraEntity.get<Nz::NodeComponent>();
		cameraNode.SetRotation(camAngles);
		cameraNode.SetPosition(target - cameraNode.GetForward() * camDistance);
	};
	UpdateCamera();

	NazaraSlot(Nz::WindowEventHandler, OnMouseMoved, cameraMove);
	NazaraSlot(Nz::WindowEventHandler, OnMouseMoved, grabbedObjectMove);

	struct GrabConstraint
	{
		GrabConstraint(Nz::RigidBody3D& body, const Nz::Vector3f& grabPos) :
		grabBody(body.GetWorld(), BodySettings(grabPos)),
		grabConstraint(body, grabBody, grabPos)
		{
			body.WakeUp();
			body.EnableSleeping(false);
			grabConstraint.SetDamping(1.f);
			grabConstraint.SetFrequency(5.f);
		}

		~GrabConstraint()
		{
			grabConstraint.GetBodyA().EnableSleeping(true);
		}

		void SetPosition(const Nz::Vector3f& pos)
		{
			grabBody.SetPosition(pos);
		}

		static Nz::RigidBody3D::DynamicSettings BodySettings(const Nz::Vector3f& pos)
		{
			Nz::RigidBody3D::DynamicSettings settings;
			settings.mass = 0.f; //< kinematic
			settings.isSimulationEnabled = false;
			settings.position = pos;

			return settings;
		}

		Nz::RigidBody3D grabBody;
		Nz::PhysDistanceConstraint3D grabConstraint;
	};

	std::optional<GrabConstraint> grabConstraint;

	auto mouseMoveCallback = [&](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseMoveEvent& event)
	{
		constexpr float sensitivity = 0.3f;

		camAngles.yaw -= event.deltaX * sensitivity;
		camAngles.yaw.Normalize();

		camAngles.pitch = Nz::Clamp(camAngles.pitch - event.deltaY * sensitivity, -89.f, 89.f);
		UpdateCamera();
	};

	Nz::WindowEventHandler& eventHandler = mainWindow.GetEventHandler();
	eventHandler.OnMouseButtonPressed.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseButtonEvent& event)
	{
		if (event.button == Nz::Mouse::Middle)
		{
			cameraMove.Connect(eventHandler.OnMouseMoved, mouseMoveCallback);
		}
		else if (event.button == Nz::Mouse::Left)
		{
			auto& cameraComponent = cameraEntity.get<Nz::CameraComponent>();

			Nz::Vector3f from = cameraComponent.Unproject({ float(event.x), float(event.y), 0.f });
			Nz::Vector3f to = cameraComponent.Unproject({ float(event.x), float(event.y), 1.f });

			Nz::Physics3DSystem::RaycastHit lastHitInfo;
			auto callback = [&](const decltype(lastHitInfo)& hitInfo) -> std::optional<float>
			{
				if (hitInfo.hitEntity == boxColliderEntity)
				{
					Nz::Vector3f dirToCenter = Nz::Vector3f::Zero() - hitInfo.hitPosition;
					dirToCenter.Normalize();

					if (Nz::Vector3f::DotProduct(dirToCenter, hitInfo.hitNormal) < 0.f)
						return std::nullopt;
				}

				lastHitInfo = hitInfo;

				return hitInfo.fraction;
			};

			if (physSystem.RaycastQuery(from, to, callback))
			{
				if (lastHitInfo.hitBody && lastHitInfo.hitEntity != boxColliderEntity)
				{
					grabConstraint.emplace(static_cast<Nz::RigidBody3D&>(*lastHitInfo.hitBody), lastHitInfo.hitPosition);

					grabbedObjectMove.Connect(eventHandler.OnMouseMoved, [&, distance = Nz::Vector3f::Distance(from, lastHitInfo.hitPosition)](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseMoveEvent& event)
					{
						Nz::Vector3f from = cameraComponent.Unproject({ float(event.x), float(event.y), 0.f });
						Nz::Vector3f to = cameraComponent.Unproject({ float(event.x), float(event.y), 1.f });

						Nz::Vector3f newPosition = from + (to - from).Normalize() * distance;
						grabConstraint->SetPosition(newPosition);
					});
				}
				else
					cameraMove.Connect(eventHandler.OnMouseMoved, mouseMoveCallback);
			}
			else
				cameraMove.Connect(eventHandler.OnMouseMoved, mouseMoveCallback);
		}
	});

	eventHandler.OnMouseButtonReleased.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseButtonEvent& event)
	{
		if (event.button == Nz::Mouse::Left)
		{
			grabConstraint.reset();
			cameraMove.Disconnect();
			grabbedObjectMove.Disconnect();
		}
		else if (event.button == Nz::Mouse::Middle)
			cameraMove.Disconnect();
	});

	eventHandler.OnMouseWheelMoved.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseWheelEvent& event)
	{
		camDistance = Nz::Clamp(camDistance - event.delta, 5.f, 20.f);
		UpdateCamera();
	});

	eventHandler.OnKeyReleased.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::KeyEvent& event)
	{
		if (event.virtualKey == Nz::Keyboard::VKey::G)
		{
			auto& cameraNode = cameraEntity.get<Nz::NodeComponent>();

			physSystem.GetPhysWorld().SetGravity(cameraNode.GetBackward() * 9.81f);
		}
	});

	Nz::DegreeAnglef rotation = 0.f;
	app.AddUpdaterFunc([&](Nz::Time elapsedTime)
	{
		rotation += elapsedTime.AsSeconds() * 45.f;
		//physSystem.GetPhysWorld().SetGravity(Nz::Quaternionf(Nz::EulerAnglesf(0.f, rotation, 0.f)) * Nz::Vector3f::Forward() * 10.f);
	});

	Nz::MillisecondClock fpsClock;
	unsigned int fps = 0;
	app.AddUpdaterFunc([&]
	{
		fps++;
		if (fpsClock.RestartIfOver(Nz::Time::Second()))
		{
			mainWindow.SetTitle("Physics playground - " + Nz::NumberToString(fps) + " FPS" + " - " + Nz::NumberToString(world.GetAliveEntityCount()) + " entities");
			fps = 0;
		}
	});

	return app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

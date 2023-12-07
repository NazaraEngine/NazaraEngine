// Sources pour https://github.com/NazaraEngine/NazaraEngine/wiki/(FR)-Tutoriel:-%5B01%5D-Hello-World

#define USE_JOLT 1

#include <Nazara/Core.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Platform/AppWindowingComponent.hpp>
#if USE_JOLT
#include <Nazara/JoltPhysics3D.hpp>
#else
#include <Nazara/BulletPhysics3D.hpp>
#endif
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <iostream>
#include <random>

constexpr float BoxDims = 16.f;

int main(int argc, char* argv[])
{
	try {
	// Mise en place de l'application, de la fenêtre et du monde
	Nz::Renderer::Config renderConfig;
	renderConfig.validationLevel = Nz::RenderAPIValidationLevel::None;

#if USE_JOLT
	Nz::Application<Nz::Graphics, Nz::JoltPhysics3D> app(argc, argv, renderConfig);
#else
	Nz::Application<Nz::Graphics, Nz::BulletPhysics3D> app(argc, argv, renderConfig);
#endif

	auto& windowing = app.AddComponent<Nz::AppWindowingComponent>();
	Nz::Window& mainWindow = windowing.CreateWindow(Nz::VideoMode(1280, 720), "Physics playground");

	auto& fs = app.AddComponent<Nz::AppFilesystemComponent>();
	{
		std::filesystem::path resourceDir = "assets/examples";
		if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
			resourceDir = "../.." / resourceDir;

		fs.Mount("assets", resourceDir);
	}

	auto& ecs = app.AddComponent<Nz::AppEntitySystemComponent>();
	auto& world = ecs.AddWorld<Nz::EnttWorld>();

#if USE_JOLT
	auto& physSystem = world.AddSystem<Nz::JoltPhysics3DSystem>();
#else
	auto& physSystem = world.AddSystem<Nz::BulletPhysics3DSystem>();
#endif
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

		Nz::TextureParams textureParams = *fs.GetDefaultResourceParameters<Nz::Texture>();
		textureParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

		Nz::TextureSamplerInfo planeSampler;
		planeSampler.anisotropyLevel = 16;
		planeSampler.wrapModeU = Nz::SamplerWrap::Repeat;
		planeSampler.wrapModeV = Nz::SamplerWrap::Repeat;

		std::shared_ptr<Nz::MaterialInstance> boxMat = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Phong);
		boxMat->SetTextureProperty("BaseColorMap", fs.Load<Nz::Texture>("assets/dev_grey.png", textureParams), planeSampler);
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

#if USE_JOLT
		float thickness = 1.f;
		std::shared_ptr<Nz::JoltBoxCollider3D> wallCollider = std::make_shared<Nz::JoltBoxCollider3D>(Nz::Vector3f(BoxDims + thickness * 2.f, BoxDims + thickness * 2.f, thickness));
#else
		std::shared_ptr<Nz::BulletBoxCollider3D> wallCollider = std::make_shared<Nz::BulletBoxCollider3D>(Nz::Vector3f(BoxDims, BoxDims, 1.f));
#endif

#if USE_JOLT
		std::vector<Nz::JoltCompoundCollider3D::ChildCollider> colliders;
#else
		std::vector<Nz::BulletCompoundCollider3D::ChildCollider> colliders;
#endif

		for (Nz::Vector3f normal : { Nz::Vector3f::Forward(), Nz::Vector3f::Backward(), Nz::Vector3f::Left(), Nz::Vector3f::Right(), Nz::Vector3f::Up(), Nz::Vector3f::Down() })
		{
			auto& colliderEntry = colliders.emplace_back();
			colliderEntry.collider = wallCollider;
#if USE_JOLT
			colliderEntry.rotation = Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), normal);
			colliderEntry.offset = normal * BoxDims * 0.5f + normal * 0.5f;
#else
			colliderEntry.offsetMatrix = Nz::Matrix4f::Transform(normal * BoxDims * 0.5f + normal * 0.5f, Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), normal));
#endif
		}

#if USE_JOLT
		std::shared_ptr<Nz::JoltCompoundCollider3D> boxCollider = std::make_shared<Nz::JoltCompoundCollider3D>(std::move(colliders));
#else
		std::shared_ptr<Nz::BulletCompoundCollider3D> boxCollider = std::make_shared<Nz::BulletCompoundCollider3D>(std::move(colliders));
#endif

#if USE_JOLT
		Nz::JoltRigidBody3D::StaticSettings settings;
		settings.geom = boxCollider;

		boxColliderEntity.emplace<Nz::JoltRigidBody3DComponent>(settings);
#else
		auto& boxBody = boxColliderEntity.emplace<Nz::BulletRigidBody3DComponent>(physSystem.CreateRigidBody(boxCollider));
		boxBody.SetMass(0.f);
#endif

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

#if USE_JOLT
		std::shared_ptr<Nz::JoltSphereCollider3D> sphereCollider = std::make_shared<Nz::JoltSphereCollider3D>(radius);
#else
		std::shared_ptr<Nz::BulletSphereCollider3D> sphereCollider = std::make_shared<Nz::BulletSphereCollider3D>(radius);
#endif

		entt::handle ballEntity = world.CreateEntity();

		std::shared_ptr<Nz::MaterialInstance> ballMaterial = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Phong);
		ballMaterial->SetValueProperty("BaseColor", Nz::Color::sRGBToLinear(Nz::Color::FromHSV(colorDis(rd), 1.f, 1.f)));

		std::shared_ptr<Nz::Model> sphereModel = std::make_shared<Nz::Model>(sphereMesh);
		sphereModel->SetMaterial(0, std::move(ballMaterial));

		auto& ballGfx = ballEntity.emplace<Nz::GraphicsComponent>();
		ballGfx.AttachRenderable(std::move(sphereModel));

		auto& ballNode = ballEntity.emplace<Nz::NodeComponent>();
		ballNode.SetPosition(positionRandom(rd), positionRandom(rd), positionRandom(rd));
		ballNode.SetScale(radius);

#if USE_JOLT
		Nz::JoltRigidBody3D::DynamicSettings settings;
		settings.geom = sphereCollider;
		settings.mass = 4.f / 3.f * Nz::Pi<float> * Nz::IntegralPow(radius, 3);

		ballEntity.emplace<Nz::JoltRigidBody3DComponent>(settings);
#else
		ballEntity.emplace<Nz::BulletRigidBody3DComponent>(physSystem.CreateRigidBody(sphereCollider));
#endif
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
		ballNode.SetPosition(xRandom(rd), yRandom(rd), zRandom(rd));
		ballNode.SetScale(width, height, depth);

#if USE_JOLT
		std::shared_ptr<Nz::JoltBoxCollider3D> boxCollider = std::make_shared<Nz::JoltBoxCollider3D>(Nz::Vector3f(width, height, depth));
#else
		std::shared_ptr<Nz::BulletBoxCollider3D> boxCollider = std::make_shared<Nz::BulletBoxCollider3D>(Nz::Vector3f(width, height, depth));
#endif

#if USE_JOLT
		Nz::JoltRigidBody3D::DynamicSettings settings;
		settings.geom = boxCollider;
		settings.mass = width * height * depth;

		boxEntity.emplace<Nz::JoltRigidBody3DComponent>(settings);
#else
		boxEntity.emplace<Nz::BulletRigidBody3DComponent>(physSystem.CreateRigidBody(boxCollider));
#endif
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

		Nz::TextureParams texParams;
		texParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

		material->SetTextureProperty("BaseColorMap", fs.Load<Nz::Texture>("assets/Spaceship/Texture/diffuse.png", texParams));

		std::shared_ptr<Nz::Model> model = std::make_shared<Nz::Model>(std::move(gfxMesh));
		for (std::size_t i = 0; i < model->GetSubMeshCount(); ++i)
			model->SetMaterial(i, material);

		Nz::VertexMapper vertexMapper(*spaceshipMesh->GetSubMesh(0));
		Nz::SparsePtr<Nz::Vector3f> vertices = vertexMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent::Position);

#if USE_JOLT
		auto shipCollider = std::make_shared<Nz::JoltConvexHullCollider3D>(vertices, vertexMapper.GetVertexCount(), 0.1f);
#else
		auto shipCollider = std::make_shared<Nz::BulletConvexCollider3D>(vertices, vertexMapper.GetVertexCount());
#endif

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
			shipNode.SetPosition(xRandom(rd), yRandom(rd), zRandom(rd));

#if USE_JOLT
			Nz::JoltRigidBody3D::DynamicSettings settings;
			settings.geom = shipCollider;
			settings.mass = 100.f;

			shipEntity.emplace<Nz::JoltRigidBody3DComponent>(settings);
#else
			shipEntity.emplace<Nz::BulletRigidBody3DComponent>(physSystem.CreateRigidBody(shipCollider));
#endif

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
#if USE_JOLT
	struct GrabConstraint
	{
		GrabConstraint(Nz::JoltRigidBody3D& body, const Nz::Vector3f& grabPos) :
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

		static Nz::JoltRigidBody3D::DynamicSettings BodySettings(const Nz::Vector3f& pos)
		{
			Nz::JoltRigidBody3D::DynamicSettings settings;
			settings.mass = 0.f; //< kinematic
			settings.isSimulationEnabled = false;
			settings.position = pos;

			return settings;
		}

		Nz::JoltRigidBody3D grabBody;
		Nz::JoltDistanceConstraint3D grabConstraint;
	};

	std::optional<GrabConstraint> grabConstraint;
#else
	std::optional<Nz::BulletPivotConstraint3D> grabConstraint;
#endif

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

#if USE_JOLT
			Nz::JoltPhysics3DSystem::RaycastHit lastHitInfo;
#else
			Nz::BulletPhysics3DSystem::RaycastHit lastHitInfo;
#endif
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
#if USE_JOLT
					grabConstraint.emplace(static_cast<Nz::JoltRigidBody3D&>(*lastHitInfo.hitBody), lastHitInfo.hitPosition);
#else
					grabConstraint.emplace(*lastHitInfo.hitBody, lastHitInfo.hitPosition);
					grabConstraint->SetImpulseClamp(30.f);
#endif

					grabbedObjectMove.Connect(eventHandler.OnMouseMoved, [&, distance = Nz::Vector3f::Distance(from, lastHitInfo.hitPosition)](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseMoveEvent& event)
					{
						Nz::Vector3f from = cameraComponent.Unproject({ float(event.x), float(event.y), 0.f });
						Nz::Vector3f to = cameraComponent.Unproject({ float(event.x), float(event.y), 1.f });

						Nz::Vector3f newPosition = from + (to - from).Normalize() * distance;
#if USE_JOLT
						grabConstraint->SetPosition(newPosition);
#else
						grabConstraint->SetSecondAnchor(newPosition);
#endif
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

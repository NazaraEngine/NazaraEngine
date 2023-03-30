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

int main()
{
	try {
	// Mise en place de l'application, de la fenêtre et du monde
	Nz::Renderer::Config renderConfig;
	renderConfig.validationLevel = Nz::RenderAPIValidationLevel::None;

#if USE_JOLT
	Nz::Application<Nz::Graphics, Nz::JoltPhysics3D> app(renderConfig);
#else
	Nz::Application<Nz::Graphics, Nz::BulletPhysics3D> app(renderConfig);
#endif

	auto& windowing = app.AddComponent<Nz::AppWindowingComponent>();
	Nz::Window& mainWindow = windowing.CreateWindow(Nz::VideoMode(1920, 1080), "Physics playground");

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

	entt::handle boxEntity = world.CreateEntity();
	{
		std::shared_ptr<Nz::GraphicalMesh> boxMesh = Nz::GraphicalMesh::Build(Nz::Primitive::Box(Nz::Vector3f(BoxDims), Nz::Vector3ui::Zero(), Nz::Matrix4f::Scale(Nz::Vector3f(-1.f)), Nz::Rectf(0.f, 0.f, 2.f, 2.f)));

		Nz::TextureSamplerInfo planeSampler;
		planeSampler.anisotropyLevel = 16;
		planeSampler.wrapModeU = Nz::SamplerWrap::Repeat;
		planeSampler.wrapModeV = Nz::SamplerWrap::Repeat;

		std::shared_ptr<Nz::MaterialInstance> boxMat = Nz::Graphics::Instance()->GetDefaultMaterials().phongMaterial->Instantiate();
		boxMat->SetTextureProperty("BaseColorMap", fs.Load<Nz::Texture>("assets/dev_grey.png"), planeSampler);
		boxMat->DisablePass("ShadowPass");
		boxMat->UpdatePassesStates([&](Nz::RenderStates& states)
		{
			states.frontFace = Nz::FrontFace::Clockwise;
		});

		std::shared_ptr<Nz::Model> boxModel = std::make_shared<Nz::Model>(std::move(boxMesh));
		boxModel->SetMaterial(0, std::move(boxMat));

		auto& boxGfx = boxEntity.emplace<Nz::GraphicsComponent>();
		boxGfx.AttachRenderable(std::move(boxModel));

		boxEntity.emplace<Nz::NodeComponent>();

#if USE_JOLT
		std::shared_ptr<Nz::JoltBoxCollider3D> wallCollider = std::make_shared<Nz::JoltBoxCollider3D>(Nz::Vector3f(BoxDims, BoxDims, 1.f));
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
		auto& ballPhysics = boxEntity.emplace<Nz::JoltRigidBody3DComponent>(physSystem.CreateRigidBody(boxCollider));
#else
		auto& ballPhysics = boxEntity.emplace<Nz::BulletRigidBody3DComponent>(physSystem.CreateRigidBody(boxCollider));
#endif
		ballPhysics.SetMass(0.f);

		std::shared_ptr<Nz::Model> colliderModel;
		{
			std::shared_ptr<Nz::MaterialInstance> colliderMat = Nz::Graphics::Instance()->GetDefaultMaterials().basicMaterial->Instantiate();
			colliderMat->SetValueProperty("BaseColor", Nz::Color::Green());
			colliderMat->UpdatePassesStates([](Nz::RenderStates& states)
			{
				states.primitiveMode = Nz::PrimitiveMode::LineList;
				return true;
			});

			/*std::shared_ptr<Nz::Mesh> colliderMesh = Nz::Mesh::Build(boxCollider->GenerateDebugMesh());
			std::shared_ptr<Nz::GraphicalMesh> colliderGraphicalMesh = Nz::GraphicalMesh::BuildFromMesh(*colliderMesh);

			colliderModel = std::make_shared<Nz::Model>(colliderGraphicalMesh);
			for (std::size_t i = 0; i < colliderModel->GetSubMeshCount(); ++i)
				colliderModel->SetMaterial(i, colliderMat);

			boxGfx.AttachRenderable(std::move(colliderModel));*/
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

		std::shared_ptr<Nz::MaterialInstance> ballMaterial = Nz::Graphics::Instance()->GetDefaultMaterials().phongMaterial->Instantiate();
		ballMaterial->SetValueProperty("BaseColor", Nz::Color::FromHSV(colorDis(rd), 1.f, 1.f));

		std::shared_ptr<Nz::Model> sphereModel = std::make_shared<Nz::Model>(sphereMesh);
		sphereModel->SetMaterial(0, std::move(ballMaterial));

		auto& ballGfx = ballEntity.emplace<Nz::GraphicsComponent>();
		ballGfx.AttachRenderable(std::move(sphereModel));

		auto& ballNode = ballEntity.emplace<Nz::NodeComponent>();
		ballNode.SetPosition(positionRandom(rd), positionRandom(rd), positionRandom(rd));
		ballNode.SetScale(radius);

#if USE_JOLT
		auto& ballPhysics = ballEntity.emplace<Nz::JoltRigidBody3DComponent>(physSystem.CreateRigidBody(sphereCollider));
#else
		auto& ballPhysics = ballEntity.emplace<Nz::BulletRigidBody3DComponent>(physSystem.CreateRigidBody(sphereCollider));
#endif
		ballPhysics.SetMass(4.f / 3.f * Nz::Pi<float> * Nz::IntegralPow(radius, 3));
		//ballPhysics.DisableSleeping();
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

		entt::handle ballEntity = world.CreateEntity();

		std::shared_ptr<Nz::MaterialInstance> boxMaterial = Nz::Graphics::Instance()->GetDefaultMaterials().phongMaterial->Instantiate();
		boxMaterial->SetValueProperty("BaseColor", Nz::Color::FromHSV(colorDis(rd), 1.f, 1.f));

		std::shared_ptr<Nz::Model> sphereModel = std::make_shared<Nz::Model>(boxMesh);
		sphereModel->SetMaterial(0, std::move(boxMaterial));

		auto& ballGfx = ballEntity.emplace<Nz::GraphicsComponent>();
		ballGfx.AttachRenderable(std::move(sphereModel));

		auto& ballNode = ballEntity.emplace<Nz::NodeComponent>();
		ballNode.SetPosition(xRandom(rd), yRandom(rd), zRandom(rd));
		ballNode.SetScale(width, height, depth);

#if USE_JOLT
		std::shared_ptr<Nz::JoltBoxCollider3D> boxCollider = std::make_shared<Nz::JoltBoxCollider3D>(Nz::Vector3f(width, height, depth));
#else
		std::shared_ptr<Nz::BulletBoxCollider3D> boxCollider = std::make_shared<Nz::BulletBoxCollider3D>(Nz::Vector3f(width, height, depth));
#endif

#if USE_JOLT
		auto& ballPhysics = ballEntity.emplace<Nz::JoltRigidBody3DComponent>(physSystem.CreateRigidBody(boxCollider));
#else
		auto& ballPhysics = ballEntity.emplace<Nz::BulletRigidBody3DComponent>(physSystem.CreateRigidBody(boxCollider));
#endif
		ballPhysics.SetMass(width * height * depth);
		//ballPhysics.DisableSleeping();
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

		auto& cameraComponent = cameraEntity.emplace<Nz::CameraComponent>(&windowSwapchain, Nz::ProjectionType::Perspective);
		cameraComponent.UpdateFOV(70.f);
		cameraComponent.UpdateClearColor(Nz::Color(0.46f, 0.48f, 0.84f, 1.f));
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
	std::optional<Nz::JoltPivotConstraint3D> grabConstraint;
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
				if (hitInfo.hitEntity == boxEntity)
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
				if (lastHitInfo.hitBody && lastHitInfo.hitEntity != boxEntity)
				{
					grabConstraint.emplace(*lastHitInfo.hitBody, lastHitInfo.hitPosition);
					//grabConstraint->SetImpulseClamp(30.f);

					grabbedObjectMove.Connect(eventHandler.OnMouseMoved, [&, body = lastHitInfo.hitBody, distance = Nz::Vector3f::Distance(from, lastHitInfo.hitPosition)](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseMoveEvent& event)
					{
						Nz::Vector3f from = cameraComponent.Unproject({ float(event.x), float(event.y), 0.f });
						Nz::Vector3f to = cameraComponent.Unproject({ float(event.x), float(event.y), 1.f });

						Nz::Vector3f newPosition = from + (to - from).Normalize() * distance;
						grabConstraint->SetSecondAnchor(newPosition);
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
	app.AddUpdater([&](Nz::Time elapsedTime)
	{
		rotation += elapsedTime.AsSeconds() * 45.f;
		//physSystem.GetPhysWorld().SetGravity(Nz::Quaternionf(Nz::EulerAnglesf(0.f, rotation, 0.f)) * Nz::Vector3f::Forward() * 10.f);
	});

	Nz::MillisecondClock fpsClock;
	unsigned int fps = 0;
	app.AddUpdater([&](Nz::Time /*elapsedTime*/)
	{
		fps++;
		if (fpsClock.RestartIfOver(Nz::Time::Second()))
		{
			mainWindow.SetTitle("Physics playground - " + Nz::NumberToString(fps) + " FPS" + " - " + Nz::NumberToString(world.GetRegistry().alive()) + " entities");
			physSystem.Dump();
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

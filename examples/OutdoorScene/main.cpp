#include <Nazara/Core.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <NZSL/Math/FieldOffsets.hpp>
#include <entt/entt.hpp>
#include <array>
#include <bitset>
#include <iostream>
#include <limits>

NAZARA_REQUEST_DEDICATED_GPU()

int main(int argc, char* argv[])
{
	constexpr std::size_t s_cascadeCount = 3;

	Nz::Application<Nz::Graphics> app(argc, argv);

	auto& ecs = app.AddComponent<Nz::AppEntitySystemComponent>();

	auto& world = ecs.AddWorld<Nz::EnttWorld>();
	Nz::RenderSystem& renderSystem = world.AddSystem<Nz::RenderSystem>();

	auto& windowing = app.AddComponent<Nz::AppWindowingComponent>();

	std::string windowTitle = "Outdoor scene";
	Nz::Window& mainWindow = windowing.CreateWindow(Nz::VideoMode(1920, 1080), windowTitle);
	auto& windowSwapchain = renderSystem.CreateSwapchain(mainWindow);

	//mainWindow.SetStayOnTop(true);

	auto& fs = app.AddComponent<Nz::AppFilesystemComponent>();
	{
		std::filesystem::path resourceDir = "assets/examples";
		if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
			resourceDir = "../.." / resourceDir;

		fs.Mount("assets", resourceDir);
	}

	entt::handle sunLightEntity = world.CreateEntity();
	{
		auto& lightNode = sunLightEntity.emplace<Nz::NodeComponent>();
		lightNode.SetRotation(Nz::EulerAnglesf(-70.f, 0.f, 0.f));

		auto& cameraLight = sunLightEntity.emplace<Nz::LightComponent>();

		auto& dirLight = cameraLight.AddLight<Nz::DirectionalLight>();
		dirLight.UpdateColor(Nz::Color::White());
		dirLight.EnableShadowCasting(true);
	}

	Nz::FixedVector<Nz::Frustumf, s_cascadeCount> cascadeFrustums;

	entt::handle camera = world.CreateEntity();
	{
		auto& cameraNode = camera.emplace<Nz::NodeComponent>();
		cameraNode.SetPosition(Nz::Vector3f::Up() * 2.f + Nz::Vector3f::Backward());

		auto& cameraComponent = camera.emplace<Nz::CameraComponent>(std::make_shared<Nz::RenderWindow>(windowSwapchain));
		cameraComponent.UpdateZNear(0.2f);
		cameraComponent.UpdateZFar(200.f);
		cameraComponent.UpdateRenderMask(1);
		cameraComponent.UpdateClearColor(Nz::Color(0.5f, 0.5f, 0.5f));
	}

	entt::handle floorEntity = world.CreateEntity();
	{
		Nz::MeshParams meshPrimitiveParams;
		meshPrimitiveParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV);

		Nz::Vector2f planeSize(25.f, 25.f);

		std::shared_ptr<Nz::GraphicalMesh> planeMeshGfx = Nz::GraphicalMesh::Build(Nz::Primitive::Plane(planeSize, Nz::Vector2ui(0u), Nz::Matrix4f::Identity(), Nz::Rectf(0.f, 0.f, 10.f, 10.f)), meshPrimitiveParams);

		Nz::TextureParams texParams;
		texParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

		Nz::TextureSamplerInfo planeSampler;
		planeSampler.anisotropyLevel = 16;
		planeSampler.wrapModeU = Nz::SamplerWrap::Repeat;
		planeSampler.wrapModeV = Nz::SamplerWrap::Repeat;

		std::shared_ptr<Nz::MaterialInstance> planeMat = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Phong);
		planeMat->SetTextureProperty("BaseColorMap", fs.Load<Nz::Texture>("assets/Grass/grass1-albedo3.png", texParams), planeSampler);
		planeMat->SetTextureProperty("NormalMap", fs.Load<Nz::Texture>("assets/Grass/grass1-normal1-ogl.png"), planeSampler);
		planeMat->SetValueProperty("Shininess", 20.f);

		std::shared_ptr<Nz::Model> planeModel = std::make_shared<Nz::Model>(std::move(planeMeshGfx));
		planeModel->SetMaterial(0, planeMat);

		auto& planeGfx = floorEntity.emplace<Nz::GraphicsComponent>();
		planeGfx.AttachRenderable(planeModel);

		floorEntity.emplace<Nz::NodeComponent>();
	}

	entt::handle sphereEntity = world.CreateEntity();
	{
		std::shared_ptr<Nz::Mesh> sphereMesh = Nz::Mesh::Build(Nz::Primitive::UVSphere(1.f, 50, 50));

		std::shared_ptr<Nz::GraphicalMesh> gfxMesh = Nz::GraphicalMesh::BuildFromMesh(*sphereMesh);

		Nz::TextureParams texParams;
		texParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

		std::shared_ptr<Nz::MaterialInstance> sphereMat = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Phong);
		sphereMat->SetTextureProperty("BaseColorMap", fs.Load<Nz::Texture>("assets/Rusty/rustediron2_basecolor.png", texParams));

		std::shared_ptr<Nz::Model> sphereModel = std::make_shared<Nz::Model>(std::move(gfxMesh));
		for (std::size_t i = 0; i < sphereModel->GetSubMeshCount(); ++i)
			sphereModel->SetMaterial(i, sphereMat);

		auto& sphereNode = sphereEntity.emplace<Nz::NodeComponent>();
		sphereNode.SetPosition(Nz::Vector3f::Up() * 2.f);
		sphereNode.SetScale(1.f);

		auto& sphereGfx = sphereEntity.emplace<Nz::GraphicsComponent>();
		sphereGfx.AttachRenderable(sphereModel);
	}

	Nz::MillisecondClock fpsClock, updateClock;

	Nz::EulerAnglesf camAngles = Nz::EulerAnglesf(0.f, 0.f, 0.f);
	Nz::UInt64 fps = 0;

	Nz::WindowEventHandler& eventHandler = mainWindow.GetEventHandler();

	eventHandler.OnMouseMoved.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseMoveEvent& event)
	{
		// Gestion de la caméra free-fly (Rotation)
		float sensitivity = 0.3f; // Sensibilité de la souris

		// On modifie l'angle de la caméra grâce au déplacement relatif sur X de la souris
		camAngles.yaw = camAngles.yaw - event.deltaX * sensitivity;
		camAngles.yaw.Normalize();

		// Idem, mais pour éviter les problèmes de calcul de la matrice de vue, on restreint les angles
		camAngles.pitch = Nz::Clamp(camAngles.pitch - event.deltaY * sensitivity, -89.f, 89.f);

		auto& playerRotNode = camera.get<Nz::NodeComponent>();
		playerRotNode.SetRotation(camAngles);
	});

	float nearFactor = 0.f;

	app.AddUpdaterFunc([&]
	{
		if (std::optional<Nz::Time> deltaTime = updateClock.RestartIfOver(Nz::Time::TickDuration(60)))
		{
			float updateTime = deltaTime->AsSeconds();

			float cameraSpeed = 10.f;

			auto& cameraNode = camera.get<Nz::NodeComponent>();
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Space))
				cameraNode.Move(Nz::Vector3f::Up() * cameraSpeed * updateTime, Nz::CoordSys::Global);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Z))
				cameraNode.Move(Nz::Vector3f::Forward() * cameraSpeed * updateTime, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::S))
				cameraNode.Move(Nz::Vector3f::Backward() * cameraSpeed * updateTime, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Q))
				cameraNode.Move(Nz::Vector3f::Left() * cameraSpeed * updateTime, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::D))
				cameraNode.Move(Nz::Vector3f::Right() * cameraSpeed * updateTime, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::F5))
			{
				auto& cameraComponent = camera.get<Nz::CameraComponent>();

				std::array<float, s_cascadeCount - 1> planePct = { 0.3f, 0.8f };

				cascadeFrustums.clear();

				Nz::Matrix4f viewMatrix = cameraComponent.GetViewerInstance().GetViewMatrix();
				Nz::Matrix4f projMatrix = Nz::Matrix4f::Perspective(cameraComponent.GetFOV(), 16.f / 9.f, 0.1f, 20.f);

				Nz::Frustumf cameraFrustum = Nz::Frustumf::Extract(viewMatrix * projMatrix);
				//Nz::Frustumf cameraFrustum = Nz::Frustumf::Extract(cameraComponent.GetViewerInstance().GetViewProjMatrix());
				cameraFrustum.Split(planePct.data(), planePct.size(), [&](float zNear, float zFar)
				{
					cascadeFrustums.push_back(cameraFrustum.Reduce(zNear, zFar));
				});
			}
		}

		constexpr std::array<Nz::Color, s_cascadeCount> frustumColor = { Nz::Color::Green(), Nz::Color::Yellow(), Nz::Color::Red() };

		Nz::DebugDrawer& debugDrawer = renderSystem.GetFramePipeline().GetDebugDrawer();
		//if (!cascadeFrustums.empty())
		//	debugDrawer.DrawFrustum(cascadeFrustums.front().Reduce(nearFactor, 1.f), Nz::Color::Blue());
		for (std::size_t i = 0; i < cascadeFrustums.size(); ++i)
		{
			Nz::Boxf frustumAABB = cascadeFrustums[i].GetAABB();

			Nz::Quaternionf lightDir = sunLightEntity.get<Nz::NodeComponent>().GetRotation();

			Nz::Matrix4f lightView = Nz::Matrix4f::TransformInverse(frustumAABB.GetCenter(), lightDir);

			float left = std::numeric_limits<float>::infinity();
			float right = -std::numeric_limits<float>::infinity();
			float top = std::numeric_limits<float>::infinity();
			float bottom = -std::numeric_limits<float>::infinity();
			float znear = std::numeric_limits<float>::infinity();
			float zfar = -std::numeric_limits<float>::infinity();
			for (Nz::Vector3f corner : frustumAABB.GetCorners())
			{
				corner = lightView * corner;

				left = std::min(left, corner.x);
				right = std::max(right, corner.x);
				top = std::min(top, corner.y);
				bottom = std::max(bottom, corner.y);
				znear = std::min(znear, corner.z);
				zfar = std::max(zfar, corner.z);
			}

			Nz::Matrix4f lightProj = Nz::Matrix4f::Ortho(left, right, top, bottom, znear, zfar);

			Nz::Frustumf lightFrustum = Nz::Frustumf::Extract(lightView * lightProj);

			debugDrawer.DrawFrustum(lightFrustum, frustumColor[i]);
			debugDrawer.DrawBox(frustumAABB, frustumColor[i]);
			debugDrawer.DrawPoint(frustumAABB.GetCenter(), frustumColor[i]);
		}

		fps++;

		if (fpsClock.RestartIfOver(Nz::Time::Second()))
		{
			mainWindow.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS" + " - " + Nz::NumberToString(world.GetAliveEntityCount()) + " entities");
			fps = 0;
		}
	});

	return app.Run();
}

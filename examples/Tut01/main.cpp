// Sources pour https://github.com/NazaraEngine/NazaraEngine/wiki/(FR)-Tutoriel:-%5B01%5D-Hello-World

#include <Nazara/Graphics.hpp>
#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Graphics/Systems.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <Nazara/Utility/Components.hpp>
#include <entt/entt.hpp>
#include <chrono>
#include <iostream>
#include <thread>

int main()
{
	Nz::Modules<Nz::Graphics> nazara;

	Nz::RenderWindow mainWindow(Nz::Graphics::Instance()->GetRenderDevice(), Nz::VideoMode(1280, 720, 32), "Test");

	entt::registry registry;
	Nz::RenderSystem renderSystem(registry);

	entt::entity cameraEntity = registry.create();
	{
		registry.emplace<Nz::NodeComponent>(cameraEntity);
		auto& cameraComponent = registry.emplace<Nz::CameraComponent>(cameraEntity, mainWindow.GetRenderTarget(), Nz::ProjectionType::Orthographic);
		cameraComponent.UpdateClearColor(Nz::Color(0.46f, 0.48f, 0.84f, 1.f));
	}

	Nz::SimpleTextDrawer textDrawer;
	textDrawer.SetCharacterSize(72);
	textDrawer.SetOutlineThickness(4.f);
	textDrawer.SetText("Hello world !");

	std::shared_ptr<Nz::Material> material = std::make_shared<Nz::Material>();

	std::shared_ptr<Nz::MaterialPass> materialPass = std::make_shared<Nz::MaterialPass>(Nz::BasicMaterial::GetSettings());
	materialPass->EnableDepthBuffer(true);
	materialPass->EnableDepthWrite(false);
	materialPass->EnableScissorTest(true);
	materialPass->EnableBlending(true);
	materialPass->SetBlendEquation(Nz::BlendEquation::Add, Nz::BlendEquation::Add);
	materialPass->SetBlendFunc(Nz::BlendFunc::SrcAlpha, Nz::BlendFunc::InvSrcAlpha, Nz::BlendFunc::One, Nz::BlendFunc::One);

	material = std::make_shared<Nz::Material>();
	material->AddPass("ForwardPass", materialPass);

	std::shared_ptr<Nz::TextSprite> textSprite = std::make_shared<Nz::TextSprite>(material);
	textSprite->Update(textDrawer);

	entt::entity textEntity = registry.create();
	{
		auto& nodeComponent = registry.emplace<Nz::NodeComponent>(textEntity);
		auto& gfxComponent = registry.emplace<Nz::GraphicsComponent>(textEntity);
		gfxComponent.AttachRenderable(textSprite, 0xFFFFFFFF);

		Nz::Boxf textBox = textSprite->GetAABB();
		Nz::Vector2ui windowSize = mainWindow.GetSize();
		nodeComponent.SetPosition(windowSize.x / 2 - textBox.width / 2, windowSize.y / 2 - textBox.height / 2);
	}

	while (mainWindow.IsOpen())
	{
		mainWindow.ProcessEvents();

		Nz::RenderFrame renderFrame = mainWindow.AcquireFrame();
		if (!renderFrame)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}

		renderSystem.Render(registry, renderFrame);

		renderFrame.Present();
	}

	return EXIT_SUCCESS;
}

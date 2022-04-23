// Sources pour https://github.com/DigitalPulseSoftware/NazaraEngine/wiki/(FR)-Tutoriel:-%5B02%5D-Gestion-des-événements

#include <Nazara/Graphics.hpp>
#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Graphics/Systems.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <Nazara/Utility/Components.hpp>
#include <entt/entt.hpp>
#include <iostream>

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
	textDrawer.SetText("Press a key");

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

	Nz::EventHandler& eventHandler = mainWindow.GetEventHandler();
	eventHandler.OnKeyPressed.Connect([&](const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& e)
	{
		textDrawer.SetText("You pressed " + Nz::Keyboard::GetKeyName(e.virtualKey));
		textSprite->Update(textDrawer);

		Nz::Boxf textBox = textSprite->GetAABB();
		Nz::Vector2ui windowSize = mainWindow.GetSize();

		auto& nodeComponent = registry.get<Nz::NodeComponent>(textEntity);
		nodeComponent.SetPosition(windowSize.x / 2 - textBox.width / 2, windowSize.y / 2 - textBox.height / 2);

		// Profitons-en aussi pour nous donner un moyen de quitter le programme
		if (e.virtualKey == Nz::Keyboard::VKey::Escape)
			mainWindow.Close(); // Cette ligne casse la boucle de la fenêtre
	});

	while (mainWindow.IsOpen())
	{
		mainWindow.ProcessEvents();

		Nz::RenderFrame renderFrame = mainWindow.AcquireFrame();
		renderSystem.Render(registry, renderFrame);

		renderFrame.Present();
	}

	return EXIT_SUCCESS;
}

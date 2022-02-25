#include <Nazara/Core.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Graphics/Systems.hpp>
#include <Nazara/Math/PidController.hpp>
#include <Nazara/Physics3D.hpp>
#include <Nazara/Physics3D/Components.hpp>
#include <Nazara/Physics3D/Systems.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Shader.hpp>
#include <Nazara/Shader/SpirvConstantCache.hpp>
#include <Nazara/Shader/SpirvPrinter.hpp>
#include <Nazara/Utility.hpp>
#include <Nazara/Utility/Components.hpp>
#include <Nazara/Widgets.hpp>
#include <entt/entt.hpp>
#include <array>
#include <iostream>
#include <limits>

NAZARA_REQUEST_DEDICATED_GPU()

int main()
{
	std::filesystem::path resourceDir = "resources";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory(".." / resourceDir))
		resourceDir = ".." / resourceDir;

	Nz::Renderer::Config rendererConfig;
	std::cout << "Run using Vulkan? (y/n)" << std::endl;
	if (std::getchar() != 'n')
		rendererConfig.preferredAPI = Nz::RenderAPI::Vulkan;
	else
		rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL;

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	Nz::Modules<Nz::Graphics, Nz::Physics3D, Nz::Widgets> nazara(rendererConfig);

	Nz::RenderWindow window;

	std::shared_ptr<Nz::RenderDevice> device = Nz::Graphics::Instance()->GetRenderDevice();

	std::string windowTitle = "Widget Test";
	if (!window.Create(device, Nz::VideoMode(1920, 1080, 32), windowTitle))
	{
		std::cout << "Failed to create Window" << std::endl;
		return __LINE__;
	}

	entt::registry registry;
	Nz::RenderSystem renderSystem(registry);

	Nz::Canvas canvas2D(registry, window.GetEventHandler(), window.GetCursorController().CreateHandle(), 0xFFFFFFFF);
	canvas2D.Resize(Nz::Vector2f(window.GetSize()));

	Nz::LabelWidget* labelWidget = canvas2D.Add<Nz::LabelWidget>();
	labelWidget->SetPosition(0.f, 200.f, 0.f);
	labelWidget->UpdateText(Nz::SimpleTextDrawer::Draw("Je suis un LabelWidget !", 72));

	unsigned int clickCount = 0;

	Nz::ButtonWidget* buttonWidget = canvas2D.Add<Nz::ButtonWidget>();
	buttonWidget->SetPosition(200.f, 400.f);
	buttonWidget->UpdateText(Nz::SimpleTextDrawer::Draw("Press me senpai", 72));
	buttonWidget->Resize(buttonWidget->GetPreferredSize());
	buttonWidget->OnButtonTrigger.Connect([&](const Nz::ButtonWidget*)
	{
		labelWidget->UpdateText(Nz::SimpleTextDrawer::Draw("You clicked the button " + std::to_string(++clickCount) + " times", 72));
	});

	std::shared_ptr<Nz::Material> material = std::make_shared<Nz::Material>();

	std::shared_ptr<Nz::MaterialPass> materialPass = std::make_shared<Nz::MaterialPass>(Nz::BasicMaterial::GetSettings());
	material->AddPass("ForwardPass", materialPass);

	Nz::TextureSamplerInfo samplerInfo;
	samplerInfo.anisotropyLevel = 8;

	Nz::TextureParams texParams;
	texParams.renderDevice = device;
	texParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

	Nz::BasicMaterial basicMat(*materialPass);
	basicMat.SetDiffuseMap(Nz::Texture::LoadFromFile(resourceDir / "Spaceship/Texture/diffuse.png", texParams));
	basicMat.SetDiffuseSampler(samplerInfo);

	Nz::ImageWidget* imageWidget = canvas2D.Add<Nz::ImageWidget>();
	imageWidget->SetPosition(1200.f, 200.f);
	imageWidget->SetMaterial(material);
	imageWidget->Resize(imageWidget->GetPreferredSize() / 4.f);

	Nz::TextAreaWidget* textAreaWidget = canvas2D.Add<Nz::TextAreaWidget>();
	textAreaWidget->SetPosition(800.f, 500.f);
	textAreaWidget->SetText("Je suis un TextAreaWidget !");
	textAreaWidget->Resize(Nz::Vector2f(400.f, textAreaWidget->GetPreferredHeight() * 5.f));
	textAreaWidget->SetBackgroundColor(Nz::Color::White);
	textAreaWidget->SetTextColor(Nz::Color::Black);
	textAreaWidget->EnableMultiline(true);

	Nz::CheckboxWidget* checkboxWidget = canvas2D.Add<Nz::CheckboxWidget>();
	//checkboxWidget->EnableTristate(true);
	checkboxWidget->SetPosition(800.f, 800.f);
	checkboxWidget->Resize({ 256.f, 256.f });
	checkboxWidget->SetState(true);

	/*Nz::TextAreaWidget* textAreaWidget2 = canvas2D.Add<Nz::TextAreaWidget>();
	textAreaWidget2->SetPosition(800.f, 700.f);
	textAreaWidget2->SetText("Je suis un autre TextAreaWidget !");
	textAreaWidget2->Resize(Nz::Vector2f(500.f, textAreaWidget2->GetPreferredHeight()));
	textAreaWidget2->SetBackgroundColor(Nz::Color::White);
	textAreaWidget2->SetTextColor(Nz::Color::Black);*/

	entt::entity viewer2D = registry.create();
	{
		registry.emplace<Nz::NodeComponent>(viewer2D);
		auto& cameraComponent = registry.emplace<Nz::CameraComponent>(viewer2D, window.GetRenderTarget(), Nz::ProjectionType::Orthographic);
		cameraComponent.UpdateClearColor(Nz::Color(173, 216, 230, 255));
	}

	window.EnableEventPolling(true);

	Nz::Clock updateClock;
	Nz::Clock secondClock;
	unsigned int fps = 0;

	float elapsedTime = 0.f;
	Nz::UInt64 time = Nz::GetElapsedMicroseconds();

	while (window.IsOpen())
	{
		Nz::UInt64 now = Nz::GetElapsedMicroseconds();
		elapsedTime = (now - time) / 1'000'000.f;
		time = now;

		Nz::WindowEvent event;
		while (window.PollEvent(&event))
		{
			switch (event.type)
			{
				case Nz::WindowEventType::Quit:
					window.Close();
					break;

				default:
					break;
			}
		}

		Nz::RenderFrame frame = window.AcquireFrame();
		if (!frame)
			continue;

		renderSystem.Render(registry, frame);

		frame.Present();

		fps++;

		if (secondClock.GetMilliseconds() >= 1000)
		{
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS" + " - " + Nz::NumberToString(registry.alive()) + " entities");

			fps = 0;

			secondClock.Restart();
		}
	}

	return EXIT_SUCCESS;
}

#include <Nazara/Core.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Math/PidController.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/TextRenderer.hpp>
#include <Nazara/Widgets.hpp>
#include <entt/entt.hpp>
#include <array>
#include <iostream>
#include <limits>

NAZARA_REQUEST_DEDICATED_GPU()

int main(int argc, char* argv[])
{
	Nz::Application<Nz::Graphics, Nz::Widgets> app(argc, argv);

	auto& windowing = app.AddComponent<Nz::WindowingAppComponent>();
	Nz::Window& mainWindow = windowing.CreateWindow(Nz::VideoMode(1920, 1080), "Widget demo");

	auto& ecs = app.AddComponent<Nz::EntitySystemAppComponent>();
	auto& world = ecs.AddWorld<Nz::EnttWorld>();

	auto& fs = app.AddComponent<Nz::FilesystemAppComponent>();
	{
		std::filesystem::path resourceDir = "assets/examples";
		if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
			resourceDir = "../.." / resourceDir;

		fs.Mount("assets", resourceDir);
	}

	Nz::RenderSystem& renderSystem = world.AddSystem<Nz::RenderSystem>();
	auto& windowSwapchain = renderSystem.CreateSwapchain(mainWindow);

	Nz::Canvas canvas2D(world, mainWindow.GetEventHandler(), mainWindow.GetCursorController().CreateHandle(), 0xFFFFFFFF);
	canvas2D.Resize(Nz::Vector2f(mainWindow.GetSize()));

	Nz::LabelWidget* labelWidget = canvas2D.Add<Nz::LabelWidget>();
	labelWidget->SetPosition({ 0.f, 200.f });
	labelWidget->UpdateText(Nz::SimpleTextDrawer::Draw("Je suis un LabelWidget !", 72));

	unsigned int clickCount = 0;

	Nz::ButtonWidget* buttonWidget = canvas2D.Add<Nz::ButtonWidget>();
	buttonWidget->SetPosition({ 200.f, 400.f });
	buttonWidget->UpdateText(Nz::SimpleTextDrawer::Draw("Press me senpai", 72));
	buttonWidget->Resize(buttonWidget->GetPreferredSize());

	Nz::TextureSamplerInfo samplerInfo;
	samplerInfo.anisotropyLevel = 8;

	std::shared_ptr<Nz::MaterialInstance> materialInstance = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Basic);
	materialInstance->SetTextureProperty("BaseColorMap", fs.Load<Nz::Texture>("assets/lynix.jpg"));

	Nz::ImageWidget* imageWidget = canvas2D.Add<Nz::ImageWidget>(materialInstance);
	imageWidget->SetPosition({ 1200.f, 200.f });
	imageWidget->Resize(imageWidget->GetPreferredSize() / 4.f);

	Nz::ImageButtonWidget* imageButtonWidget = canvas2D.Add<Nz::ImageButtonWidget>(materialInstance);
	imageButtonWidget->SetPosition({ 1400, 500.f });
	imageButtonWidget->Resize(imageButtonWidget->GetPreferredSize() / 4.f);

	Nz::TextAreaWidget* textAreaWidget = canvas2D.Add<Nz::TextAreaWidget>();
	textAreaWidget->SetPosition({ 800.f, 500.f });
	textAreaWidget->SetText("Je suis un TextAreaWidget !");
	textAreaWidget->Resize(Nz::Vector2f(400.f, textAreaWidget->GetPreferredHeight() * 5.f));
	textAreaWidget->SetBackgroundColor(Nz::Color::White());
	textAreaWidget->SetTextColor(Nz::Color::Black());
	textAreaWidget->EnableMultiline(true);

	Nz::CheckboxWidget* checkboxWidget = canvas2D.Add<Nz::CheckboxWidget>();
	//checkboxWidget->EnableTristate(true);
	checkboxWidget->SetPosition({ 800.f, 800.f });
	checkboxWidget->Resize({ 256.f, 256 });
	checkboxWidget->SetState(true);

	Nz::TextAreaWidget* longTextArea = canvas2D.Add<Nz::TextAreaWidget>();
	longTextArea->EnableLineWrap(true);
	longTextArea->EnableMultiline(true);
	longTextArea->SetBackgroundColor(Nz::Color::White());
	longTextArea->SetTextColor(Nz::Color::Black());
	longTextArea->SetText("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum");

	Nz::ScrollAreaWidget* scrollBarWidget = canvas2D.Add<Nz::ScrollAreaWidget>(longTextArea);
	scrollBarWidget->SetPosition({ 1400.f, 800.f });
	scrollBarWidget->Resize({ 512.f, 256.f });

	Nz::RichTextAreaWidget* textAreaWidget2 = canvas2D.Add<Nz::RichTextAreaWidget>();
	textAreaWidget2->EnableMultiline(true);
	textAreaWidget2->SetPosition({ 1000.f, 200.f });
	textAreaWidget2->SetBackgroundColor(Nz::Color::White());
	textAreaWidget2->SetTextColor(Nz::Color::Black());

	Nz::RichTextBuilder builder(textAreaWidget2);
	builder << Nz::Color::Blue() << "Rich " << Nz::TextStyle::Bold << "text" << Nz::TextStyle_Regular << builder.CharacterSize(36) << Nz::Color::Black() << "\nAnd a even " << builder.CharacterSize(48) << Nz::Color::Red() << "bigger" << builder.CharacterSize(24) << Nz::Color::Black() << " text";
	textAreaWidget2->Resize(Nz::Vector2f(500.f, textAreaWidget2->GetPreferredHeight()));

	Nz::ProgressBarWidget* progressBarWidget = canvas2D.Add<Nz::ProgressBarWidget>();
	progressBarWidget->SetPosition({ 200.f, 600.f });
	progressBarWidget->Resize({ 512.f, 64.f });

	buttonWidget->OnButtonTrigger.Connect([&](const Nz::ButtonWidget*)
	{
		labelWidget->UpdateText(Nz::SimpleTextDrawer::Draw("You clicked the button " + std::to_string(++clickCount) + " times", 72));
		if (progressBarWidget->GetFraction() >= 1.f)
			progressBarWidget->SetFraction(0.f);
		else
			progressBarWidget->SetFraction(progressBarWidget->GetFraction() + 0.1001f); //< ensures ten clicks go over 1
	});

	entt::handle viewer2D = world.CreateEntity();
	{
		viewer2D.emplace<Nz::NodeComponent>();

		auto& cameraComponent = viewer2D.emplace<Nz::CameraComponent>(std::make_shared<Nz::RenderWindow>(windowSwapchain), Nz::ProjectionType::Orthographic);
		cameraComponent.UpdateClearColor(Nz::Color(0.46f, 0.48f, 0.84f, 1.f));
	}

	return app.Run();
}

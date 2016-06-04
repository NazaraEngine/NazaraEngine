#include <Nazara/Graphics.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <Ndk/Application.hpp>
#include <Ndk/Components.hpp>
#include <Ndk/Systems.hpp>
#include <iostream>
#include <QApplication>
#include <QTimer>
#include <QFrame>

class NazaraCanvas : public QWidget, public Nz::RenderWindow
{
	public:
		NazaraCanvas(QWidget* parent, Ndk::World& world) :
		QWidget(parent)
		{
			// Setup some states to allow direct rendering into the widget
			setAttribute(Qt::WA_PaintOnScreen);
			setAttribute(Qt::WA_OpaquePaintEvent);
			setAttribute(Qt::WA_NoSystemBackground);

			// Set strong focus to enable keyboard events to be received
			setFocusPolicy(Qt::StrongFocus);

			// Setup the widget geometry
			move(QPoint(20, 20));
			resize(QSize(360, 360));
		}
	
	private:
		void resizeEvent(QResizeEvent*) override
		{
			OnRenderTargetSizeChange(this);
		}

		void showEvent(QShowEvent*) override
		{
			if (!IsValid())
			{
				// Under X11, we need to flush the commands sent to the server to ensure that
				// SFML will get an updated view of the windows
				#ifdef Q_WS_X11
				XFlush(QX11Info::display());
				#endif

				Nz::RenderWindow::Create(reinterpret_cast<Nz::WindowHandle>(winId()));
			}
		}

		QPaintEngine* paintEngine() const override
		{
			return nullptr;
		}

		void paintEvent(QPaintEvent*) override
		{
		}
};

int main(int argc, char *argv[])
{
	Ndk::Application ndkApp;
	ndkApp.MakeExitOnLastWindowClosed(false);

	QApplication App(argc, argv);
	// Create the main frame
	QFrame* MainFrame = new QFrame;
	MainFrame->setWindowTitle("Qt Nazara");
	MainFrame->resize(400, 400);
	MainFrame->show();

	Ndk::World& world = ndkApp.AddWorld();

	NazaraCanvas* NazaraView = new NazaraCanvas(MainFrame, world);
	NazaraView->show();

	world.GetSystem<Ndk::RenderSystem>().SetGlobalUp(Nz::Vector3f::Down());
	world.GetSystem<Ndk::RenderSystem>().SetDefaultBackground(Nz::ColorBackground::New(Nz::Color(192, 100, 100)));


	Ndk::EntityHandle viewEntity = world.CreateEntity();
	viewEntity->AddComponent<Ndk::NodeComponent>();

	Ndk::CameraComponent& viewer = viewEntity->AddComponent<Ndk::CameraComponent>();
	viewer.SetTarget(NazaraView);
	viewer.SetProjectionType(Nz::ProjectionType_Orthogonal);


	Nz::TextSpriteRef textSprite = Nz::TextSprite::New();
	textSprite->Update(Nz::SimpleTextDrawer::Draw("Hello world !", 72));

	Ndk::EntityHandle text = world.CreateEntity();
	Ndk::NodeComponent& nodeComponent = text->AddComponent<Ndk::NodeComponent>();

	Ndk::GraphicsComponent& graphicsComponent = text->AddComponent<Ndk::GraphicsComponent>();
	graphicsComponent.Attach(textSprite);

	Nz::Boxf textBox = graphicsComponent.GetBoundingVolume().aabb;
	nodeComponent.SetPosition(NazaraView->GetWidth() / 2 - textBox.width / 2, NazaraView->GetHeight() / 2 - textBox.height / 2);

	NazaraView->SetEventListener(true);

	QTimer worldUpdate;
	worldUpdate.setInterval(0);
	worldUpdate.connect(&worldUpdate, &QTimer::timeout, [&] () 
	{ 
		if (ndkApp.Run())
		{
			Nz::WindowEvent event;
			while (NazaraView->PollEvent(&event))
			{
				std::cout << event.type << std::endl;
			}

			NazaraView->Display();
		}
		else
			QApplication::quit();
	});
	worldUpdate.start();

	return App.exec();
}

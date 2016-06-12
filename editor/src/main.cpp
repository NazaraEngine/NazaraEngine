#include <Nazara/Graphics.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <Ndk/Application.hpp>
#include <Ndk/Components.hpp>
#include <Ndk/Systems.hpp>
#include <iostream>
#include <QApplication>
#include <QtGui>
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
			move(QPoint(0, 0));
			resize(QSize(360, 360));
		}

		unsigned int GetHeight() const override
		{
			return height();
		}

		unsigned int GetWidth() const override
		{
			return width();
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

		virtual void keyPressEvent(QKeyEvent* key) override
		{
			std::cout << (char) key->key() << std::endl;
		}
};

int main(int argc, char *argv[])
{
	Ndk::Application ndkApp;
	ndkApp.MakeExitOnLastWindowClosed(false);

	QApplication App(argc, argv);
	// Create the main frame
	QFrame* MainFrame = new QFrame;
	MainFrame->setWindowTitle("Nazara Model Importer");
	MainFrame->resize(400, 400);
	MainFrame->show();

	Ndk::World& world = ndkApp.AddWorld();

	NazaraCanvas* NazaraView = new NazaraCanvas(MainFrame, world);
	NazaraView->show();
	NazaraView->resize(400, 400);

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
				switch (event.type)
				{
					case Nz::WindowEventType_MouseMoved:
						std::cout << event.mouseMove.x << ", " << event.mouseMove.y << std::endl;
						break;

					case Nz::WindowEventType_KeyPressed:
						std::cout << event.key.code << std::endl;
						break;
				}
			}

			NazaraView->Display();
		}
		else
			QApplication::quit();
	});
	worldUpdate.start();

	return App.exec();
}

#include "GameState.hpp"

#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Platform/Window.hpp>
#include <Nazara/Utility.hpp>
#include <Nazara/Graphics.hpp>

#include <iostream>
#include <format>

const Nz::Vector2f PAD_SIZE					= Nz::Vector2f(10.f, 40.f);
const Nz::Vector2f BALL_SIZE				= Nz::Vector2f(10.f, 10.f);

const int PAD_COLLISION_ID = 1;
const int BALL_COLLISION_ID = 2;
const int BORDER_COLLISION_ID = 3;

GameState::GameState(Nz::EnttWorld& world, Nz::Window& window) : State(), m_windowSize(window.GetSize()), m_world(world){
	m_ball = CreateEntity(world, m_windowSize / 2.f - BALL_SIZE / 2.f, "Ball");
	m_leftPad = CreateEntity(world, Nz::Vector2f(10.f, m_windowSize.y / 2.f - PAD_SIZE.y / 2.f), "Pad");
	m_rightPad = CreateEntity(world, Nz::Vector2f(m_windowSize.x - PAD_SIZE.x - 10, m_windowSize.y / 2.f - PAD_SIZE.y / 2.f), "Pad");

	m_upperBord = CreateEntity(world, Nz::Vector2f(0, m_windowSize.y - 10), "HorizontalBorder");
	m_lowerBord = CreateEntity(world, Nz::Vector2f(0, 0), "HorizontalBorder");
	m_rightBord = CreateEntity(world, Nz::Vector2f(m_windowSize.x - 10.f, 0), "VerticalBorder");
	m_leftBord = CreateEntity(world, Nz::Vector2f(0, 0), "VerticalBorder");
	
	m_score = world.CreateEntity();
	
	AddKeyboard(window);
	ResetBall();
}


void GameState::Enter(Nz::StateMachine& fsm)
{
}

void GameState::Leave(Nz::StateMachine& fsm)
{
}

bool GameState::Update(Nz::StateMachine& fsm, Nz::Time elapsedTime)
{
	checkPhysics();
	return true;
}

entt::handle GameState::CreateEntity(Nz::EnttWorld& world, const Nz::Vector2f& position, const std::string& name) {
	entt::handle entity = CreateBaseEntity(world, position);

	Nz::GraphicsComponent& graphicsComponent = entity.emplace<Nz::GraphicsComponent>();
	auto materialInstance = Nz::Graphics::Instance()->GetDefaultMaterials().basicMaterial->Instantiate();
	std::shared_ptr<Nz::Sprite> sprite = std::make_shared<Nz::Sprite>(materialInstance);

	if (name == "Pad") {
		sprite->SetSize(PAD_SIZE);
		sprite->SetColor(Nz::Color(1.f, 1.f, 1.f));
	}
	else if (name == "Ball"){
		sprite->SetSize(BALL_SIZE);
		sprite->SetColor(Nz::Color(1.f, 0.f, 0.f));
	}
	else if (name == "HorizontalBorder") {
		sprite->SetSize(HOZIRONTAL_BORD_SIZE);
		sprite->SetColor(Nz::Color(0.741, 0.765, 0.780));
	}
	else if (name == "VerticalBorder") {
		sprite->SetSize(VERTICAL_BORD_SIZE);
		sprite->SetColor(Nz::Color(0.741, 0.765, 0.780));
	}
	graphicsComponent.AttachRenderable(sprite);

	Nz::VelocityComponent& velocityComponent = entity.emplace<Nz::VelocityComponent>();
	std::cout << std::format("current velocity : {}",velocityComponent.GetLinearVelocity().ToString()) << std::endl;
	
	return entity;
}

entt::handle GameState::CreateBaseEntity(Nz::EnttWorld& world, const Nz::Vector2f& position) {
	entt::handle entity = world.CreateEntity();
	Nz::NodeComponent& nodeComponent = entity.emplace<Nz::NodeComponent>();
	nodeComponent.SetPosition(position);
	return entity;
}


void GameState::ResetBall() {
	UpdateScore();
	
	Nz::NodeComponent& nodeComponent = m_ball.get<Nz::NodeComponent>();
	nodeComponent.SetPosition(m_windowSize / 2.f - BALL_SIZE / 2.f);
	std::srand((unsigned int)time(NULL));
	bool leftOrRight = std::rand() % 2;
	// We make our ball go between an angle of -45 to +45 or 135 to 225
	float angle = std::rand() % 90 - 45.f;
	if (leftOrRight)
		angle += 180.f;

	Nz::VelocityComponent& velocityComponent = m_ball.get<Nz::VelocityComponent>();
	
	velocityComponent.UpdateLinearVelocity(500.f * Nz::Vector2f(std::cos(Nz::DegreeToRadian(angle)), -std::sin(Nz::DegreeToRadian(angle))));
}

bool isInCollision(entt::handle& obj1, entt::handle& obj2) {
	auto nodeComponentObj1 = obj1.get<Nz::NodeComponent>();
	auto PosObj1 = nodeComponentObj1.GetPosition();
	
	auto nodeComponentObj2 = obj2.get<Nz::NodeComponent>();
	auto PosObj2 = nodeComponentObj2.GetPosition();
	
	auto graphicsComponentObj1 = obj1.get<Nz::GraphicsComponent>();
	auto BoxObj1 = graphicsComponentObj1.GetAABB();

	auto graphicsComponentObj2 = obj2.get<Nz::GraphicsComponent>();
	auto BoxObj2 = graphicsComponentObj2.GetAABB();

	if (PosObj1.x <= PosObj2.x + BoxObj2.width && PosObj1.x + BoxObj1.width>= PosObj2.x) {
		if (PosObj1.y <= PosObj2.y + BoxObj2.height && PosObj1.y + BoxObj1.height >= PosObj2.y) {
			return true;
		}
	}
	return false;
}

void GameState::BounceBallY() {
	std::cout << "bournce y" << std::endl;
	auto& velocityComponent = m_ball.get<Nz::VelocityComponent>();
	auto velocity = velocityComponent.GetLinearVelocity();
	velocity.y = -velocity.y;
	velocityComponent.UpdateLinearVelocity(velocity);
}

void GameState::BounceBallX() {
	std::cout << "bounce x" << std::endl;
	auto& velocityComponent = m_ball.get<Nz::VelocityComponent>();
	auto velocity = velocityComponent.GetLinearVelocity();
	velocity.x = -velocity.x;
	velocityComponent.UpdateLinearVelocity(velocity);
}

void GameState::checkPhysics() {
	if (isInCollision(m_ball, m_leftPad) || isInCollision(m_ball, m_rightPad)) {
		BounceBallX();
	} 
	else if (isInCollision(m_ball, m_upperBord) || isInCollision(m_ball, m_lowerBord)) {
		BounceBallY();
	}
	else if (isInCollision(m_ball, m_leftBord)) {
		PlayerBScore++;
		ResetBall();
	}
	else if (isInCollision(m_ball, m_rightBord)) {
		PlayerAScore++;
		ResetBall();
	}
}

void GameState::UpdateScore() {
	m_score.destroy();
	m_score = m_world.CreateEntity();
	
	Nz::SimpleTextDrawer textDrawer;
	textDrawer.SetText(std::format("{} - {}", PlayerAScore, PlayerBScore));
	textDrawer.SetCharacterSize(72);
	textDrawer.SetOutlineThickness(4.f);

	std::shared_ptr<Nz::TextSprite> textSprite = std::make_shared<Nz::TextSprite>();
	textSprite->Update(textDrawer);


	auto& gfxComponent = m_score.get_or_emplace<Nz::GraphicsComponent>();
	gfxComponent.AttachRenderable(textSprite);


	auto& nodeComponent = m_score.get_or_emplace<Nz::NodeComponent>();
	nodeComponent.SetPosition(Nz::Vector2f(m_windowSize.x / 2 - gfxComponent.GetAABB().width / 2, m_windowSize.y - 10 - gfxComponent.GetAABB().height));
}

void GameState::AddKeyboard(Nz::Window& window) {
	Nz::WindowEventHandler& eventHandler = window.GetEventHandler();

	Nz::VelocityComponent& rightPad = m_rightPad.get<Nz::VelocityComponent>();
	Nz::VelocityComponent& leftPad = m_leftPad.get<Nz::VelocityComponent>();

	eventHandler.OnKeyPressed.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::KeyEvent& key) {
		float velocity = 500.f;
		if (key.scancode == Nz::Keyboard::Scancode::Up)
			rightPad.UpdateLinearVelocity(Nz::Vector2f(0.f, velocity));
		else if (key.scancode == Nz::Keyboard::Scancode::Down)
			rightPad.UpdateLinearVelocity(Nz::Vector2f(0.f, -velocity));
		
		if (key.virtualKey == Nz::Keyboard::VKey::Z)
			leftPad.UpdateLinearVelocity(Nz::Vector2f(0.f, velocity));
		else if (key.virtualKey == Nz::Keyboard::VKey::S)
			leftPad.UpdateLinearVelocity(Nz::Vector2f(0.f, -velocity));
	});

	eventHandler.OnKeyReleased.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::KeyEvent& key) {
		if (key.scancode == Nz::Keyboard::Scancode::Up)
			rightPad.UpdateLinearVelocity(Nz::Vector2f(0.f, 0.f));
		else if (key.scancode == Nz::Keyboard::Scancode::Down)
			rightPad.UpdateLinearVelocity(Nz::Vector2f(0.f, 0.f));
		
		if (key.virtualKey == Nz::Keyboard::VKey::Z)
			leftPad.UpdateLinearVelocity(Nz::Vector2f(0.f, 0.f));
		else if (key.virtualKey == Nz::Keyboard::VKey::S)
			leftPad.UpdateLinearVelocity(Nz::Vector2f(0.f, 0.f));
	});
}


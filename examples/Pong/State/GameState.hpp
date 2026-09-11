#pragma once

#include <Nazara/Core.hpp>
#include <Nazara/Platform/Window.hpp>

class GameState : public Nz::State
{
public:
    GameState(Nz::EnttWorld& world, Nz::Window& window);
    void Enter(Nz::StateMachine& fsm) override;
    void Leave(Nz::StateMachine& fsm) override;
    bool Update(Nz::StateMachine& fsm, Nz::Time elapsedTime) override;

private:
    entt::handle CreateEntity(Nz::EnttWorld& world, const Nz::Vector2f& position, const std::string& imageName);
    entt::handle CreateBaseEntity(Nz::EnttWorld& world, const Nz::Vector2f& position);
    void ResetBall();
    void checkPhysics();
    void BounceBallY();
    void BounceBallX();
    void AddKeyboard(Nz::Window& window);
    void UpdateScore();
	
    entt::handle m_ball;
	entt::handle m_leftPad;
	entt::handle m_rightPad;

    entt::handle m_upperBord;
    entt::handle m_lowerBord;
    entt::handle m_leftBord;
    entt::handle m_rightBord;

    entt::handle m_score;

    int PlayerBScore = 0;
    int PlayerAScore = 0;

    const Nz::Vector2f m_windowSize;

    Nz::EnttWorld& m_world;
};

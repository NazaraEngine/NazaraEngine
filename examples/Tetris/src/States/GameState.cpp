#include "States/GameState.hpp"
#include <Nazara/Utility/Window.hpp>
#include <NDK/Application.hpp>
#include <NDK/StateMachine.hpp>
#include <NDK/World.hpp>

///// *****************
#include "Components/BlockComponent.hpp"
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <Nazara/Graphics/Sprite.hpp>

namespace Tet {
    GameState::GameState(Nz::Window& window, Ndk::World& world)
        : m_window{ window },
        m_world{ world }
    {}

    void GameState::Enter(Ndk::StateMachine& fsm) {
        BlockComponent bc;
        Nz::TextureRef tex{ bc.GenerateTextureFromColor(Nz::Color::Red) };
        Nz::SpriteRef spr{ Nz::Sprite::New(tex.Get()) };
        Ndk::EntityHandle ent{ m_world.CreateEntity() };
        ent->AddComponent<Ndk::NodeComponent>();
        ent->AddComponent<Ndk::GraphicsComponent>().Attach(spr);
        for (const auto& i : m_world.GetEntities())
            NazaraNotice(i.ToString());
    }

    void GameState::Leave(Ndk::StateMachine& fsm) {

    }

    bool GameState::Update(Ndk::StateMachine& fsm, float elapsedTime) {
        Nz::WindowEvent event;
        while (m_window.PollEvent(&event)) {
            switch (event.type)
            {
                case Nz::WindowEventType_Quit:
                    Ndk::Application::Instance()->Quit();
                    break;
                default:
                    break;
            }
        }

        return true;
    }
}
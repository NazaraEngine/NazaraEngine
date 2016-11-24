#include "Text.hpp"

#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <NDK/StateMachine.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>

Text::Text(StateContext& stateContext) :
m_context(stateContext)
{
	m_text = m_context.world.CreateEntity();
	Ndk::NodeComponent& nodeComponent = m_text->AddComponent<Ndk::NodeComponent>();
	Ndk::GraphicsComponent& graphicsComponent = m_text->AddComponent<Ndk::GraphicsComponent>();

	m_textSprite = Nz::TextSprite::New();
	graphicsComponent.Attach(m_textSprite);
}

Text::~Text()
{
	m_textSprite->Clear();
	m_text->Kill();
}

void Text::SetContent(const Nz::String& string, unsigned int size)
{
	m_textSprite->Update(Nz::SimpleTextDrawer::Draw(string, size));
}

void Text::SetVisible(bool shouldBeVisible)
{
	m_text->Enable(shouldBeVisible);
}
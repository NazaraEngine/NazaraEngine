#include "StateFactory.hpp"

#include "MenuState.hpp"
#include "EventState.hpp"
#include "FocusState.hpp"
#include "KeyState.hpp"
#include "MouseClickState.hpp"
#include "MouseEnterState.hpp"
#include "MouseMoveState.hpp"
#include "TextEnterState.hpp"
#include "WindowModificationState.hpp"

std::shared_ptr<Ndk::State> StateFactory::Get(EventStatus state)
{
	return s_states[state];
}

std::shared_ptr<Ndk::State> StateFactory::Get(unsigned int state)
{
	NazaraAssert(state < s_states.size(), "State out of range");
	auto it = s_states.begin();
	std::advance(it, state);
	return it->second;
}

bool StateFactory::Initialize(StateContext& context)
{
	s_states.emplace(std::make_pair(EventStatus::Menu, std::make_shared<MenuState>(context)));
	s_states.emplace(std::make_pair(EventStatus::Event, std::make_shared<EventState>(context)));
	s_states.emplace(std::make_pair(EventStatus::Focus, std::make_shared<FocusState>(context)));
	s_states.emplace(std::make_pair(EventStatus::Key, std::make_shared<KeyState>(context)));
	s_states.emplace(std::make_pair(EventStatus::MouseClick, std::make_shared<MouseClickState>(context)));
	s_states.emplace(std::make_pair(EventStatus::MouseEnter, std::make_shared<MouseEnterState>(context)));
	s_states.emplace(std::make_pair(EventStatus::MouseMove, std::make_shared<MouseMoveState>(context)));
	s_states.emplace(std::make_pair(EventStatus::TextEnter, std::make_shared<TextEnterState>(context)));
	s_states.emplace(std::make_pair(EventStatus::WindowModification, std::make_shared<WindowModificationState>(context)));

	return true;
}

void StateFactory::Uninitialize()
{
	s_states.clear();
}

std::map<EventStatus, std::shared_ptr<Ndk::State>> StateFactory::s_states;
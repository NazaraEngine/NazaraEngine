// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

template<typename Base>
NzListenable<Base>::NzListenable() :
m_listenersLocked(false)
{
}

template<typename Base>
template<typename L>
void NzListenable<Base>::AddListener(L* listener, void* userdata) const
{
	static_assert(std::is_base_of<typename Base::Listener, L>::value, "`listener` does not derive from a Listener");

	if (!m_listenersLocked)
		m_listeners.insert(std::make_pair(listener, userdata));
}

template<typename Base>
template<typename L>
void NzListenable<Base>::RemoveListener(L* listener) const
{
	static_assert(std::is_base_of<typename Base::Listener, L>::value, "`listener` does not derive from a Listener");

	if (!m_listenersLocked)
		m_listeners.erase(listener);
}

template<typename Base>
template<typename F, typename... Args>
void NzListenable<Base>::Notify(F callback, Args&&... args)
{
	using Listener = typename Base::Listener;

	m_listenersLocked = true;

	auto it = m_listeners.begin();
	while (it != m_listeners.end())
	{
		Listener* listener = static_cast<Listener*>(it->first);
		if (!(listener->*callback)(static_cast<Base*>(this), std::forward<Args>(args)..., it->second))
			m_listeners.erase(it++);
		else
			++it;
	}

	m_listenersLocked = false;
}

template<typename Base>
template<typename F>
void NzListenable<Base>::NotifyRelease(F callback)
{
	using Listener = typename Base::Listener;

	m_listenersLocked = true;
	for (auto& pair : m_listeners)
	{
		Listener* listener = static_cast<Listener*>(pair.first);
		(listener->*callback)(static_cast<Base*>(this), pair.second);
	}
}

#include <Nazara/Core/DebugOff.hpp>

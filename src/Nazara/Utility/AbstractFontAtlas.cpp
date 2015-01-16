// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/AbstractFontAtlas.hpp>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Utility/Debug.hpp>

NzAbstractFontAtlas::NzAbstractFontAtlas() :
m_listenersLocked(false)
{
}

NzAbstractFontAtlas::~NzAbstractFontAtlas()
{
	m_listenersLocked = true;
	for (auto& pair : m_listeners)
		pair.first->OnAtlasReleased(this, pair.second);
}

void NzAbstractFontAtlas::AddListener(Listener* listener, void* userdata) const
{
	if (!m_listenersLocked)
		m_listeners.insert(std::make_pair(listener, userdata));
}

void NzAbstractFontAtlas::RemoveListener(Listener* listener) const
{
	if (!m_listenersLocked)
		m_listeners.erase(listener);
}

void NzAbstractFontAtlas::NotifyCleared()
{
	m_listenersLocked = true;

	auto it = m_listeners.begin();
	while (it != m_listeners.end())
	{
		if (!it->first->OnAtlasCleared(this, it->second))
			m_listeners.erase(it++);
		else
			++it;
	}

	m_listenersLocked = false;
}

NzAbstractFontAtlas::Listener::~Listener() = default;

bool NzAbstractFontAtlas::Listener::OnAtlasCleared(const NzAbstractFontAtlas* atlas, void* userdata)
{
	NazaraUnused(atlas);
	NazaraUnused(userdata);

	return true;
}

void NzAbstractFontAtlas::Listener::OnAtlasReleased(const NzAbstractFontAtlas* atlas, void* userdata)
{
	NazaraUnused(atlas);
	NazaraUnused(userdata);
}

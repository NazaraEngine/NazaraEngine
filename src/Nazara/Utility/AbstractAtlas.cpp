// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/AbstractAtlas.hpp>
#include <Nazara/Utility/Debug.hpp>

NzAbstractAtlas::NzAbstractAtlas() :
m_listenersLocked(false)
{
}

NzAbstractAtlas::~NzAbstractAtlas()
{
	m_listenersLocked = true;
	for (auto& pair : m_listeners)
		pair.first->OnAtlasReleased(this, pair.second);
}

void NzAbstractAtlas::AddListener(Listener* listener, void* userdata) const
{
	if (!m_listenersLocked)
		m_listeners.insert(std::make_pair(listener, userdata));
}

void NzAbstractAtlas::RemoveListener(Listener* listener) const
{
	if (!m_listenersLocked)
		m_listeners.erase(listener);
}

void NzAbstractAtlas::NotifyCleared()
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

void NzAbstractAtlas::NotifyLayerChange(NzAbstractImage* oldLayer, NzAbstractImage* newLayer)
{
	m_listenersLocked = true;

	auto it = m_listeners.begin();
	while (it != m_listeners.end())
	{
		if (!it->first->OnAtlasLayerChange(this, oldLayer, newLayer, it->second))
			m_listeners.erase(it++);
		else
			++it;
	}

	m_listenersLocked = false;
}


NzAbstractAtlas::Listener::~Listener() = default;

bool NzAbstractAtlas::Listener::OnAtlasCleared(const NzAbstractAtlas* atlas, void* userdata)
{
	NazaraUnused(atlas);
	NazaraUnused(userdata);

	return true;
}

bool NzAbstractAtlas::Listener::OnAtlasLayerChange(const NzAbstractAtlas* atlas, NzAbstractImage* oldLayer, NzAbstractImage* newLayer, void* userdata)
{
	NazaraUnused(atlas);
	NazaraUnused(oldLayer);
	NazaraUnused(newLayer);
	NazaraUnused(userdata);

	return true;
}

void NzAbstractAtlas::Listener::OnAtlasReleased(const NzAbstractAtlas* atlas, void* userdata)
{
	NazaraUnused(atlas);
	NazaraUnused(userdata);
}

// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ResourceListener.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <typeinfo>
#include <Nazara/Core/Debug.hpp>

NzResource::NzResource(bool persistent) :
m_resourcePersistent(persistent),
m_resourceReferenceCount(0),
m_resourceListenersLocked(false)
{
}

NzResource::~NzResource()
{
	m_resourceListenersLocked = true;
	for (auto& pair : m_resourceListeners)
		pair.first->OnResourceReleased(this, pair.second);

	#if NAZARA_CORE_SAFE
	if (m_resourceReferenceCount > 0)
		NazaraWarning("Resource destroyed while still referenced " + NzString::Number(m_resourceReferenceCount) + " time(s)");
	#endif
}

void NzResource::AddResourceListener(NzResourceListener* listener, int index) const
{
	///DOC: Est ignoré si appelé depuis un évènement
	NazaraLock(m_mutex)

	if (!m_resourceListenersLocked)
		m_resourceListeners.insert(std::make_pair(listener, index));
}

void NzResource::AddResourceReference() const
{
	m_resourceReferenceCount++;
}

unsigned int NzResource::GetResourceReferenceCount() const
{
	return m_resourceReferenceCount;
}

bool NzResource::IsPersistent() const
{
	return m_resourcePersistent;
}

void NzResource::RemoveResourceListener(NzResourceListener* listener) const
{
	///DOC: Est ignoré si appelé depuis un évènement
	NazaraLock(m_mutex);

	if (!m_resourceListenersLocked)
		m_resourceListeners.erase(listener);
}

bool NzResource::RemoveResourceReference() const
{
	#if NAZARA_CORE_SAFE
	if (m_resourceReferenceCount == 0)
	{
		NazaraError("Impossible to remove reference (Ref. counter is already 0)");
		return false;
	}
	#endif

	if (--m_resourceReferenceCount == 0 && !m_resourcePersistent)
	{
		delete this; // Suicide

		return true;
	}
	else
		return false;
}

bool NzResource::SetPersistent(bool persistent, bool checkReferenceCount)
{
	m_resourcePersistent = persistent;

	if (checkReferenceCount && !persistent && m_resourceReferenceCount == 0)
	{
		delete this;

		return true;
	}
	else
		return false;
}

void NzResource::NotifyCreated()
{
	NazaraLock(m_mutex)

	m_resourceListenersLocked = true;

	auto it = m_resourceListeners.begin();
	while (it != m_resourceListeners.end())
	{
		if (!it->first->OnResourceCreated(this, it->second))
			m_resourceListeners.erase(it++);
		else
			++it;
	}

	m_resourceListenersLocked = false;
}

void NzResource::NotifyDestroy()
{
	NazaraLock(m_mutex)

	m_resourceListenersLocked = true;

	auto it = m_resourceListeners.begin();
	while (it != m_resourceListeners.end())
	{
		if (!it->first->OnResourceDestroy(this, it->second))
			m_resourceListeners.erase(it++);
		else
			++it;
	}

	m_resourceListenersLocked = false;
}

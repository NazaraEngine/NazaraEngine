// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ResourceListener.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
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
		pair.first->OnResourceReleased(this, pair.second.first);

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
	{
		auto pair = m_resourceListeners.insert(std::make_pair(listener, std::make_pair(index, 1U)));
		if (!pair.second)
			pair.first->second.second++;
	}
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
	{
		ResourceListenerMap::iterator it = m_resourceListeners.find(listener);
		if (it != m_resourceListeners.end())
			RemoveResourceListenerIterator(it);
	}
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
		ResourceListenerMap::iterator iterator = it++;
		if (!it->first->OnResourceCreated(this, it->second.first))
			RemoveResourceListenerIterator(it++);
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
		if (!it->first->OnResourceDestroy(this, it->second.first))
			RemoveResourceListenerIterator(it++);
		else
			++it;
	}

	m_resourceListenersLocked = false;
}

void NzResource::RemoveResourceListenerIterator(ResourceListenerMap::iterator iterator) const
{
	unsigned int& referenceCount = iterator->second.second;
	if (referenceCount == 1)
		m_resourceListeners.erase(iterator);
	else
		referenceCount--;
}

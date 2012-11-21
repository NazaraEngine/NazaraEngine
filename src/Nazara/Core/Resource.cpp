// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ResourceListener.hpp>
#include <Nazara/Core/Debug.hpp>

NzResource::NzResource(bool persistent) :
m_resourcePersistent(persistent),
m_resourceReferenceCount(0)
{
}

NzResource::NzResource(const NzResource& resource) :
m_resourcePersistent(resource.m_resourcePersistent),
m_resourceReferenceCount(0)
{
}

NzResource::~NzResource()
{
	EnsureResourceListenerUpdate();
	for (const NzResourceEntry& entry : m_resourceListenersCache)
		entry.listener->OnResourceReleased(this, entry.index);

	#if NAZARA_CORE_SAFE
	if (m_resourceReferenceCount > 0)
		NazaraWarning("Resource destroyed while still referenced " + NzString::Number(m_resourceReferenceCount) + " time(s)");
	#endif
}

void NzResource::AddResourceListener(NzResourceListener* listener, int index) const
{
	NazaraLock(m_mutex)

	if (m_resourceListeners.insert(NzResourceEntry(listener, index)).second)
	{
		m_resourceListenerUpdated = false;

		// AddResourceReference()
		m_resourceReferenceCount++;
	}
}

void NzResource::AddResourceReference() const
{
	NazaraLock(m_mutex)

	m_resourceReferenceCount++;
}

bool NzResource::IsPersistent() const
{
	NazaraLock(m_mutex)

	return m_resourcePersistent;
}

void NzResource::RemoveResourceListener(NzResourceListener* listener) const
{
	NazaraMutexLock(m_mutex);

	if (m_resourceListeners.erase(listener) != 0)
		m_resourceListenerUpdated = false;
	else
		NazaraError(NzString::Pointer(listener) + " is not a listener of " + NzString::Pointer(this));

	// RemoveResourceReference()
	#if NAZARA_CORE_SAFE
	if (m_resourceReferenceCount == 0)
	{
		NazaraError("Impossible to remove reference (Ref. counter is already 0)");
		return;
	}
	#endif

	if (--m_resourceReferenceCount == 0 && !m_resourcePersistent)
	{
		NazaraMutexUnlock(m_mutex);
		delete this;
	}
	else
	{
		NazaraMutexUnlock(m_mutex);
	}
}

void NzResource::RemoveResourceReference() const
{
	NazaraMutexLock(m_mutex);

	#if NAZARA_CORE_SAFE
	if (m_resourceReferenceCount == 0)
	{
		NazaraError("Impossible to remove reference (Ref. counter is already 0)");
		return;
	}
	#endif

	if (--m_resourceReferenceCount == 0 && !m_resourcePersistent)
	{
		NazaraMutexUnlock(m_mutex);
		delete this;
	}
	else
	{
		NazaraMutexUnlock(m_mutex);
	}
}

void NzResource::SetPersistent(bool persistent)
{
	NazaraMutexLock(m_mutex);

	m_resourcePersistent = persistent;

	if (!persistent && m_resourceReferenceCount == 0)
	{
		NazaraMutexUnlock(m_mutex);
		delete this;
	}
	else
	{
		NazaraMutexUnlock(m_mutex);
	}
}

void NzResource::NotifyCreated()
{
	NazaraLock(m_mutex)

	EnsureResourceListenerUpdate();
	for (const NzResourceEntry& entry : m_resourceListenersCache)
		entry.listener->OnResourceCreated(this, entry.index);
}

void NzResource::NotifyDestroy()
{
	NazaraLock(m_mutex)

	EnsureResourceListenerUpdate();
	for (const NzResourceEntry& entry : m_resourceListenersCache)
		entry.listener->OnResourceDestroy(this, entry.index);
}

void NzResource::EnsureResourceListenerUpdate() const
{
	// Déjà bloqué par une mutex
	if (!m_resourceListenerUpdated)
	{
		m_resourceListenersCache = m_resourceListeners;
		m_resourceListenerUpdated = true;
	}
}

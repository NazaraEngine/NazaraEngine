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
m_resourceReferenceCount(0)
{
}

NzResource::NzResource(const NzResource& resource) :
m_resourcePersistent(resource.m_resourcePersistent.load()),
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

bool NzResource::RemoveResourceListener(NzResourceListener* listener) const
{
	NazaraMutexLock(m_mutex);

	if (m_resourceListeners.erase(listener) != 0)
		m_resourceListenerUpdated = false;
	else
		NazaraError(NzString::Pointer(listener) + " is not a listener of " + NzString::Pointer(this));

	return RemoveResourceReference();
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

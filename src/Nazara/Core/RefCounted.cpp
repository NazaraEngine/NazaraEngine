// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ObjectListener.hpp>

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_REFCOUNTED
	#include <Nazara/Core/ThreadSafety.hpp>
#else
	#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

#include <Nazara/Core/Debug.hpp>

NzRefCounted::NzRefCounted(bool persistent) :
m_persistent(persistent),
m_referenceCount(0),
m_objectListenersLocked(false)
{
}

NzRefCounted::~NzRefCounted()
{
	m_objectListenersLocked = true;
	for (auto& pair : m_objectListeners)
		pair.first->OnObjectReleased(this, pair.second.first);

	#if NAZARA_CORE_SAFE
	if (m_referenceCount > 0)
		NazaraWarning("Resource destroyed while still referenced " + NzString::Number(m_referenceCount) + " time(s)");
	#endif
}

void NzRefCounted::AddObjectListener(NzObjectListener* listener, int index) const
{
	///DOC: Est ignoré si appelé depuis un évènement
	NazaraLock(m_mutex)

	if (!m_objectListenersLocked)
	{
		auto pair = m_objectListeners.insert(std::make_pair(listener, std::make_pair(index, 1U)));
		if (!pair.second)
			pair.first->second.second++;
	}
}

void NzRefCounted::AddReference() const
{
	m_referenceCount++;
}

unsigned int NzRefCounted::GetReferenceCount() const
{
	return m_referenceCount;
}

bool NzRefCounted::IsPersistent() const
{
	return m_persistent;
}

void NzRefCounted::RemoveObjectListener(NzObjectListener* listener) const
{
	///DOC: Est ignoré si appelé depuis un évènement
	NazaraLock(m_mutex);

	if (!m_objectListenersLocked)
	{
		ObjectListenerMap::iterator it = m_objectListeners.find(listener);
		if (it != m_objectListeners.end())
			RemoveObjectListenerIterator(it);
	}
}

bool NzRefCounted::RemoveReference() const
{
	#if NAZARA_CORE_SAFE
	if (m_referenceCount == 0)
	{
		NazaraError("Impossible to remove reference (Ref. counter is already 0)");
		return false;
	}
	#endif

	if (--m_referenceCount == 0 && !m_persistent)
	{
		delete this; // Suicide

		return true;
	}
	else
		return false;
}

bool NzRefCounted::SetPersistent(bool persistent, bool checkReferenceCount)
{
	m_persistent = persistent;

	if (checkReferenceCount && !persistent && m_referenceCount == 0)
	{
		delete this;

		return true;
	}
	else
		return false;
}

void NzRefCounted::NotifyCreated()
{
	NazaraLock(m_mutex)

	m_objectListenersLocked = true;

	auto it = m_objectListeners.begin();
	while (it != m_objectListeners.end())
	{
		if (!it->first->OnObjectCreated(this, it->second.first))
			RemoveObjectListenerIterator(it++);
		else
			++it;
	}

	m_objectListenersLocked = false;
}

void NzRefCounted::NotifyDestroy()
{
	NazaraLock(m_mutex)

	m_objectListenersLocked = true;

	auto it = m_objectListeners.begin();
	while (it != m_objectListeners.end())
	{
		if (!it->first->OnObjectDestroy(this, it->second.first))
			RemoveObjectListenerIterator(it++);
		else
			++it;
	}

	m_objectListenersLocked = false;
}

void NzRefCounted::NotifyModified(unsigned int code)
{
	NazaraLock(m_mutex)

	m_objectListenersLocked = true;

	auto it = m_objectListeners.begin();
	while (it != m_objectListeners.end())
	{
		if (!it->first->OnObjectModified(this, it->second.first, code))
			RemoveObjectListenerIterator(it++);
		else
			++it;
	}

	m_objectListenersLocked = false;
}

void NzRefCounted::RemoveObjectListenerIterator(ObjectListenerMap::iterator iterator) const
{
	unsigned int& referenceCount = iterator->second.second;
	if (referenceCount == 1)
		m_objectListeners.erase(iterator);
	else
		referenceCount--;
}

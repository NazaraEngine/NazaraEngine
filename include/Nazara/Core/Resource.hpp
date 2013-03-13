// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RESOURCE_HPP
#define NAZARA_RESOURCE_HPP

#include <Nazara/Prerequesites.hpp>
#include <set>

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_RESOURCE
#include <Nazara/Core/ThreadSafety.hpp>
#else
#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

class NzResourceListener;

struct NzResourceEntry
{
	NzResourceEntry(NzResourceListener* resourceListener, int i = 0) :
	listener(resourceListener),
	index(i)
	{
	}

	bool operator<(const NzResourceEntry& rhs) const
	{
		return listener < rhs.listener;
	}

	NzResourceListener* listener;
	int index;
};

class NAZARA_API NzResource
{
	public:
		NzResource(bool persistent = true);
		NzResource(const NzResource& resource);
		virtual ~NzResource();

		void AddResourceListener(NzResourceListener* listener, int index = 0) const;
		void AddResourceReference() const;

		unsigned int GetResourceReferenceCount() const;

		bool IsPersistent() const;

		void RemoveResourceListener(NzResourceListener* listener) const;
		void RemoveResourceReference() const;

		void SetPersistent(bool persistent = true, bool checkReferenceCount = true);

	protected:
		void NotifyCreated();
		void NotifyDestroy();

	private:
		void EnsureResourceListenerUpdate() const;

		NazaraMutexAttrib(m_mutex, mutable)

		// Je fais précéder le nom par 'resource' pour éviter les éventuels conflits de noms
		mutable std::set<NzResourceEntry> m_resourceListeners;
		mutable std::set<NzResourceEntry> m_resourceListenersCache;
		mutable bool m_resourceListenerUpdated;
		        bool m_resourcePersistent;
		mutable unsigned int m_resourceReferenceCount;
};

#endif // NAZARA_RESOURCE_HPP

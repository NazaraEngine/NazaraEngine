// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_REFCOUNTED_HPP
#define NAZARA_REFCOUNTED_HPP

#include <Nazara/Prerequesites.hpp>
#include <atomic>
#include <unordered_map>

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_REFCOUNTED
	#include <Nazara/Core/ThreadSafety.hpp>
#else
	#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

class NzObjectListener;

class NAZARA_API NzRefCounted
{
	public:
		NzRefCounted(bool persistent = true);
		virtual ~NzRefCounted();

		void AddObjectListener(NzObjectListener* listener, int index = 0) const;
		void AddReference() const;

		unsigned int GetReferenceCount() const;

		bool IsPersistent() const;

		void RemoveObjectListener(NzObjectListener* listener) const;
		bool RemoveReference() const;

		bool SetPersistent(bool persistent = true, bool checkReferenceCount = false);

	protected:
		void NotifyCreated();
		void NotifyDestroy();
		void NotifyModified(unsigned int code);

	private:
		using ObjectListenerMap = std::unordered_map<NzObjectListener*, std::pair<int, unsigned int>>;

		void RemoveObjectListenerIterator(ObjectListenerMap::iterator iterator) const;

		NazaraMutexAttrib(m_mutex, mutable)

		mutable ObjectListenerMap m_objectListeners;
		        std::atomic_bool m_persistent;
		mutable std::atomic_uint m_referenceCount;
		        bool m_objectListenersLocked;
};

#endif // NAZARA_RESOURCE_HPP

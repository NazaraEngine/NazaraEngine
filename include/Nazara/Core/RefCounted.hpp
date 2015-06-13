// Copyright (C) 2015 Jérôme Leclercq
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

class NAZARA_CORE_API NzRefCounted
{
	public:
		NzRefCounted(bool persistent = true);
		NzRefCounted(const NzRefCounted&) = delete;
		NzRefCounted(NzRefCounted&&) = default;
		virtual ~NzRefCounted();

		void AddReference() const;

		unsigned int GetReferenceCount() const;

		bool IsPersistent() const;

		bool RemoveReference() const;

		bool SetPersistent(bool persistent = true, bool checkReferenceCount = false);

		NzRefCounted& operator=(const NzRefCounted&) = delete;
		NzRefCounted& operator=(NzRefCounted&&) = default;

	private:
		std::atomic_bool m_persistent;
		mutable std::atomic_uint m_referenceCount;
};

#endif // NAZARA_RESOURCE_HPP

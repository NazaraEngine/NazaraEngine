// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_REFCOUNTED_HPP
#define NAZARA_REFCOUNTED_HPP

#include <Nazara/Prerequisites.hpp>
#include <atomic>

namespace Nz
{
	class NAZARA_CORE_API RefCounted
	{
		public:
			RefCounted(bool persistent = true);
			RefCounted(const RefCounted&) = delete;
			RefCounted(RefCounted&&) = delete;
			virtual ~RefCounted();

			void AddReference() const;

			unsigned int GetReferenceCount() const;

			bool IsPersistent() const;

			bool RemoveReference() const;

			bool SetPersistent(bool persistent = true, bool checkReferenceCount = false);

			RefCounted& operator=(const RefCounted&) = delete;
			RefCounted& operator=(RefCounted&&) = delete;

		private:
			std::atomic_bool m_persistent;
			mutable std::atomic_uint m_referenceCount;
	};
}

#endif // NAZARA_RESOURCE_HPP

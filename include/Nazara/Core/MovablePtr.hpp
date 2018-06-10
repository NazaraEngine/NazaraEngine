// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MOVABLE_PTR_HPP
#define NAZARA_MOVABLE_PTR_HPP

namespace Nz
{
	template<typename T>
	class MovablePtr
	{
		public:
			MovablePtr(T* value = nullptr);
			MovablePtr(const MovablePtr&) = default;
			MovablePtr(MovablePtr&& ptr) noexcept;
			~MovablePtr() = default;

			T* Get() const;

			T* operator->() const;

			operator T*() const;

			MovablePtr& operator=(T* value);
			MovablePtr& operator=(const MovablePtr&) = default;
			MovablePtr& operator=(MovablePtr&& ptr) noexcept;

		private:
			T* m_value;
	};
}

#include <Nazara/Core/MovablePtr.inl>

#endif // NAZARA_MOVABLE_PTR_HPP

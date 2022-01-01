// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_MOVABLEVALUE_HPP
#define NAZARA_CORE_MOVABLEVALUE_HPP

namespace Nz
{
	template<typename T>
	class MovableValue
	{
		public:
			MovableValue(T value = T{});
			MovableValue(const MovableValue&) = default;
			MovableValue(MovableValue&& ptr) noexcept;
			~MovableValue() = default;

			T& Get();
			const T& Get() const;

			operator T&();
			operator const T&() const;

			MovableValue& operator=(T value);
			MovableValue& operator=(const MovableValue&) = default;
			MovableValue& operator=(MovableValue&& ptr) noexcept;

		private:
			T m_value;
	};
}

#include <Nazara/Core/MovableValue.inl>

#endif // NAZARA_CORE_MOVABLEVALUE_HPP

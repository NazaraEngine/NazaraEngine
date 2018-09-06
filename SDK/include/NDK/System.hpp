// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_SYSTEM_HPP
#define NDK_SYSTEM_HPP

#include <NDK/BaseSystem.hpp>

namespace Ndk
{
	template<typename SystemType>
	class System : public BaseSystem
	{
		public:
			System();
			System(const System&) = delete;
			System(System&&) noexcept = default;
			virtual ~System();

			System& operator=(const System&) = delete;
			System& operator=(System&&) noexcept = default;

			static SystemIndex RegisterSystem();
	};
}

#include <NDK/System.inl>

#endif // NDK_SYSTEM_HPP

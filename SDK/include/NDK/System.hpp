// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

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
			System(const System&) = default;
			System(System&&) = default;
			virtual ~System();

			std::unique_ptr<BaseSystem> Clone() const override;

			System& operator=(const System&) = delete;
			System& operator=(System&&) = default;

			static SystemIndex RegisterSystem();
	};
}

#include <NDK/System.inl>

#endif // NDK_SYSTEM_HPP

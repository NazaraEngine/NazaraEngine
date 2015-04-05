// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SYSTEM_HPP
#define NDK_SYSTEM_HPP

#include <NDK/BaseSystem.hpp>

namespace Ndk
{
	template<typename ComponentType>
	class System : public BaseSystem
	{
		public:
			System();
			virtual ~System();

			BaseSystem* Clone() const override;

			static SystemIndex RegisterSystem();
	};
}

#include <NDK/System.inl>

#endif // NDK_SYSTEM_HPP

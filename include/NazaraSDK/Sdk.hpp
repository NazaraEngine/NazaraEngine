// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_SDK_HPP
#define NDK_SDK_HPP

#include <Nazara/Core/ModuleBase.hpp>
#include <Nazara/Core/TypeList.hpp>
#include <NazaraSDK/Prerequisites.hpp>

#include <Nazara/Network/Network.hpp>
#include <Nazara/Physics2D/Physics2D.hpp>
#include <Nazara/Physics3D/Physics3D.hpp>
#include <Nazara/Shader/Shader.hpp>
#include <Nazara/Utility/Utility.hpp>

namespace Ndk
{
	class NDK_API Sdk : public Nz::ModuleBase<Sdk>
	{
		friend ModuleBase;

		public:
			using Dependencies = Nz::TypeList<Nz::Network, Nz::Physics2D, Nz::Physics3D, Nz::Utility>;

			struct Config {};

			Sdk(Config /*config*/);
			~Sdk();

		private:
			static Sdk* s_instance;
	};
}

#include <NazaraSDK/Sdk.inl>

#endif // NDK_SDK_HPP

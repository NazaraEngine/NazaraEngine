// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_HPP
#define NAZARA_UTILITY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API Utility
	{
		public:
			Utility() = delete;
			~Utility() = delete;

			static bool Initialize();

			static bool IsInitialized();

			static void Uninitialize();

			static unsigned int ComponentCount[ComponentType_Max+1];
			static std::size_t ComponentStride[ComponentType_Max+1];

		private:
			static unsigned int s_moduleReferenceCounter;
	};
}

#endif // NAZARA_UTILITY_HPP

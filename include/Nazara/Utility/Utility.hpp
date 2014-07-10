// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_HPP
#define NAZARA_UTILITY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Utility/Enums.hpp>

class NAZARA_API NzUtility
{
	public:
		NzUtility() = delete;
		~NzUtility() = delete;

		static bool Initialize();

		static bool IsInitialized();

		static void Uninitialize();

		static unsigned int ComponentCount[nzComponentType_Max+1];
		static std::size_t ComponentStride[nzComponentType_Max+1];

	private:
		static unsigned int s_moduleReferenceCounter;
};

#endif // NAZARA_UTILITY_HPP

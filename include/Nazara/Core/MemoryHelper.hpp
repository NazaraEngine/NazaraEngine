// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MEMORYHELPER_HPP
#define NAZARA_MEMORYHELPER_HPP

#include <cstddef>

namespace Nz
{
	void OperatorDelete(void* ptr);
	void* OperatorNew(std::size_t size);

	template<typename T, typename... Args>
	T* PlacementNew(T* ptr, Args&&... args);
}

#include <Nazara/Core/MemoryHelper.inl>

#endif // NAZARA_MEMORYHELPER_HPP

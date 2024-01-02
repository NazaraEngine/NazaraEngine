// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		inline std::size_t ComponentCounter()
		{
			static std::size_t counter = 0;
			return counter++;
		}
	}

	template<typename T>
	std::size_t ApplicationComponentRegistry<T>::GetComponentId()
	{
		static std::size_t typeId = Detail::ComponentCounter();
		return typeId;
	}
}

#include <Nazara/Core/DebugOff.hpp>

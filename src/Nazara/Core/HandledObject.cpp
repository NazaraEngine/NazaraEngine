// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		std::shared_ptr<HandleData> HandleData::GetEmptyObject()
		{
			static std::shared_ptr<HandleData> emptyHandleData = std::make_shared<HandleData>(HandleData{});
			return emptyHandleData;
		}
	}
}

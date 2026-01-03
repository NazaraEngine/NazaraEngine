// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/HandledObject.hpp>

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

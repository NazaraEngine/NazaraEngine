// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Format.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename... Args>
	void Log::Write(std::string_view str, Args&&... args)
	{
		return Write(Format(str, std::forward<Args>(args)...));
	}
}

#include <Nazara/Core/DebugOff.hpp>

// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Format.hpp>

namespace Nz
{
	template<typename... Args>
	void Log::Write(FormatString<Args...> fmt, Args&&... args)
	{
		return Write(Format(fmt, std::forward<Args>(args)...));
	}
}

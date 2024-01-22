// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

namespace Nz::PlatformImpl
{
	inline WidePathHolder PathToWideTemp(const std::filesystem::path& path)
	{
		if constexpr (ArePathWide)
			return path.native();
		else
			return path.generic_wstring();
	}
}

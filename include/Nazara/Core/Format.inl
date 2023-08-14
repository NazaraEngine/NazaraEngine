// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifdef NAZARA_BUILD
#include <fmt/format.h>
#include <fmt/std.h>
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		NAZARA_CORE_API void ClearFormatStore();
		template<typename T> void PushFormatArgImpl(const T& value);
		template<typename T> void PushFormatArg(T&& value)
		{
			PushFormatArgImpl(static_cast<const std::decay_t<T>&>(value));
		}

		NAZARA_CORE_API std::string FormatFromStore(std::string_view str);
	}

	template<typename ...Args>
	std::string Format(std::string_view str, Args&&... args)
	{
#ifdef NAZARA_BUILD
		return fmt::format(str, std::forward<Args>(args)...);
#else
		Detail::ClearFormatStore();
		(Detail::PushFormatArg(args), ...);
		return Detail::FormatFromStore(str);
#endif
	}

#undef NAZARA_TO_STRING_STD_SPEC
}

#include <Nazara/Core/DebugOff.hpp>

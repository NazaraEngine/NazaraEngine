// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


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

	NAZARA_FORCEINLINE std::string Format(FormatString<> str)
	{
#ifdef NAZARA_BUILD
		return std::string(str.get().data(), str.get().size()); // fmt::basic_string_view is different from std::string_view
#else
		return std::string(str);
#endif
	}

	template<typename... Args>
	std::string Format(FormatString<Args...> str, Args&&... args)
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


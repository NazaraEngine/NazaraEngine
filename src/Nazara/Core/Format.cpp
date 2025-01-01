// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Format.hpp>
#include <fmt/args.h>
#include <fmt/format.h>
#include <fmt/std.h>

namespace Nz
{
	namespace Detail
	{
		thread_local fmt::dynamic_format_arg_store<fmt::format_context> s_formatStore;

		void ClearFormatStore()
		{
			s_formatStore.clear();
		}

		std::string FormatFromStore(std::string_view str)
		{
			return fmt::vformat(str, s_formatStore);
		}

#define NAZARA_FORMAT_IMPLEM(Type) \
		template<> NAZARA_CORE_API void PushFormatArgImpl(Type const& value) \
		{ \
			s_formatStore.push_back(value); \
		}

		NAZARA_FORMAT_IMPLEM(std::filesystem::path);
		NAZARA_FORMAT_IMPLEM(std::string);
		NAZARA_FORMAT_IMPLEM(std::string_view);
		NAZARA_FORMAT_IMPLEM(char*);
		NAZARA_FORMAT_IMPLEM(const char*);
		NAZARA_FORMAT_IMPLEM(short);
		NAZARA_FORMAT_IMPLEM(int);
		NAZARA_FORMAT_IMPLEM(long);
		NAZARA_FORMAT_IMPLEM(long long);
		NAZARA_FORMAT_IMPLEM(unsigned short);
		NAZARA_FORMAT_IMPLEM(unsigned int);
		NAZARA_FORMAT_IMPLEM(unsigned long);
		NAZARA_FORMAT_IMPLEM(unsigned long long);
		NAZARA_FORMAT_IMPLEM(float);
		NAZARA_FORMAT_IMPLEM(double);
		NAZARA_FORMAT_IMPLEM(long double);

#undef NAZARA_FORMAT_IMPLEM
	}

#undef NAZARA_TO_STRING_CPP_SPEC
}

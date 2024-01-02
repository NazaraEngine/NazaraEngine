// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

namespace Nz
{
	constexpr std::string_view Error::TranslateFilepath(std::string_view file)
	{
		if (std::size_t offset = file.rfind('/'); offset != file.npos)
			return file.substr(offset + 1);

		if (std::size_t offset = file.rfind('\\'); offset != file.npos)
			return file.substr(offset + 1);

		return file;
	}

	inline void Error::Trigger(ErrorType type, std::string error)
	{
		return TriggerInternal(type, std::move(error), 0, {}, {});
	}

	inline void Error::Trigger(ErrorType type, unsigned int line, std::string_view file, std::string_view function, std::string error)
	{
		return TriggerInternal(type, std::move(error), line, TranslateFilepath(file), function);
	}
}

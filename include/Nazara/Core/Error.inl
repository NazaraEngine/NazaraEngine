// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

namespace Nz
{
	constexpr std::string_view Error::GetCurrentFileRelativeToEngine(std::string_view file)
	{
		if (std::size_t offset = file.find("NazaraEngine/"); offset != file.npos)
			return file.substr(offset);

		if (std::size_t offset = file.find("NazaraEngine\\"); offset != file.npos)
			return file.substr(offset);

		return file;
	}

	template<typename... Args>
	void Error::Trigger(ErrorType type, std::string_view error, Args&&... args)
	{
		return TriggerInternal(type, Format(error, std::forward<Args>(args)...), 0, {}, {});
	}

	template<typename... Args>
	void Error::Trigger(ErrorType type, unsigned int line, std::string_view file, std::string_view function, std::string_view error, Args&&... args)
	{
		return TriggerInternal(type, Format(error, std::forward<Args>(args)...), line, GetCurrentFileRelativeToEngine(file), function);
	}
}

// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline bool CommandLineParameters::GetParameter(std::string_view name, std::string_view* value) const
	{
		auto it = m_parameters.find(name);
		if (it == m_parameters.end())
			return false;

		if (value)
			*value = it->second;

		return true;
	}

	inline bool CommandLineParameters::HasFlag(std::string_view flag) const
	{
		return m_flags.contains(flag);
	}

	inline bool CommandLineParameters::operator==(const CommandLineParameters& params) const
	{
		return m_flags == params.m_flags && m_parameters == params.m_parameters;
	}

	inline bool CommandLineParameters::operator!=(const CommandLineParameters& params) const
	{
		return !operator==(params);
	}

	inline CommandLineParameters CommandLineParameters::Parse(int argc, char** argv)
	{
		return Parse(argc, const_cast<const Pointer<const char>*>(argv));
	}

	inline CommandLineParameters CommandLineParameters::Parse(int argc, const Pointer<const char>* argv)
	{
		CommandLineParameters cmdParams;

		// Parse commandline parameters
		for (int i = 1; i < argc; ++i)
		{
			const char* value = argv[i];
			if (value[0] != '-' || value[1] != '-')
				continue;

			std::string_view arg(value + 2);
			if (arg.empty())
				continue;

			std::size_t sepIdx = arg.find_first_of(":= ");
			std::string_view name = arg.substr(0, sepIdx);
			if (sepIdx != arg.npos)
				// --param=value | --param:value | "--param value"
				cmdParams.m_parameters.emplace(name, arg.substr(sepIdx + 1));
			else if (i < argc - 1)
			{
				// Check the following parameter to handle --param value
				const char* nextValue = argv[i + 1];
				if (value[0] != '-')
					cmdParams.m_parameters.emplace(name, nextValue);
				else
					cmdParams.m_flags.emplace(name);
			}
			else
				cmdParams.m_flags.emplace(name);
		}

		return cmdParams;
	}
}

#include <Nazara/Core/DebugOff.hpp>

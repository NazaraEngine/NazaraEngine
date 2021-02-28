// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/SpirvPrinter.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	inline SpirvPrinter::SpirvPrinter() :
	m_currentState(nullptr)
	{
	}

	inline std::string SpirvPrinter::Print(const std::vector<UInt32>& codepoints)
	{
		return Print(codepoints.data(), codepoints.size());
	}

	inline std::string SpirvPrinter::Print(const UInt32* codepoints, std::size_t count)
	{
		Settings settings;
		return Print(codepoints, count, settings);
	}

	inline std::string SpirvPrinter::Print(const std::vector<UInt32>& codepoints, const Settings& settings)
	{
		return Print(codepoints.data(), codepoints.size(), settings);
	}
}

#include <Nazara/Shader/DebugOff.hpp>

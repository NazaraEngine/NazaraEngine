// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline MTLParser::Material* MTLParser::AddMaterial(std::string matName)
	{
		return &m_materials[std::move(matName)];
	}

	inline void MTLParser::Clear()
	{
		m_materials.clear();
	}

	inline const MTLParser::Material* MTLParser::GetMaterial(std::string_view materialName) const
	{
		auto it = m_materials.find(materialName);
		if (it != m_materials.end())
			return &it->second;
		else
			return nullptr;
	}

	inline auto MTLParser::GetMaterials() const -> const std::unordered_map<std::string, Material, StringHash<>, std::equal_to<>>&
	{
		return m_materials;
	}

	template<typename T>
	void MTLParser::Emit(const T& text) const
	{
		m_outputStream << text;
		if (m_outputStream.rdbuf()->str().size() > 1024 * 1024)
			Flush();
	}

	inline void MTLParser::EmitLine() const
	{
		Emit('\n');
	}

	template<typename T>
	void MTLParser::EmitLine(const T& line) const
	{
		Emit(line);
		Emit('\n');
	}

	inline void MTLParser::Error(std::string_view message)
	{
		NazaraErrorFmt("{0} at line #{1}", message, m_lineCount);
	}

	inline void MTLParser::Flush() const
	{
		m_currentStream->Write(std::move(m_outputStream).str());
		m_outputStream.str({});
	}

	inline void MTLParser::Warning(std::string_view message)
	{
		NazaraWarningFmt("{0} at line #{1}", message, m_lineCount);
	}

	inline void MTLParser::UnrecognizedLine(bool error)
	{
		std::string message = "Unrecognized \"" + m_currentLine + '"';

		if (error)
			Error(message);
		else
			Warning(message);
	}
}

#include <Nazara/Core/DebugOff.hpp>

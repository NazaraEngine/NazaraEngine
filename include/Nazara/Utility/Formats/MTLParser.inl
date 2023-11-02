// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline MTLParser::Material* MTLParser::AddMaterial(const std::string& matName)
	{
		return &m_materials[matName];
	}

	inline void MTLParser::Clear()
	{
		m_materials.clear();
	}

	inline const MTLParser::Material* MTLParser::GetMaterial(const std::string& materialName) const
	{
		auto it = m_materials.find(materialName);
		if (it != m_materials.end())
			return &it->second;
		else
			return nullptr;
	}

	inline const std::unordered_map<std::string, MTLParser::Material>& MTLParser::GetMaterials() const
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

	inline void MTLParser::Error(const std::string& message)
	{
		NazaraErrorFmt("{0} at line #{1}", message, m_lineCount);
	}

	inline void MTLParser::Flush() const
	{
		m_currentStream->Write(std::move(m_outputStream).str());
		m_outputStream.str({});
	}

	inline void MTLParser::Warning(const std::string& message)
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

#include <Nazara/Utility/DebugOff.hpp>

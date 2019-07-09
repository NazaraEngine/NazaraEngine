// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline MTLParser::Material* MTLParser::AddMaterial(const String& matName)
	{
		return &m_materials[matName];
	}

	inline void MTLParser::Clear()
	{
		m_materials.clear();
	}

	inline const MTLParser::Material* MTLParser::GetMaterial(const String& materialName) const
	{
		auto it = m_materials.find(materialName);
		if (it != m_materials.end())
			return &it->second;
		else
			return nullptr;
	}

	inline const std::unordered_map<String, MTLParser::Material>& MTLParser::GetMaterials() const
	{
		return m_materials;
	}

	template<typename T>
	void MTLParser::Emit(const T& text) const
	{
		m_outputStream << text;
		if (m_outputStream.GetBufferSize() > 1024 * 1024)
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

	inline void MTLParser::Error(const String& message)
	{
		NazaraError(message + " at line #" + String::Number(m_lineCount));
	}

	inline void MTLParser::Flush() const
	{
		m_currentStream->Write(m_outputStream);
		m_outputStream.Clear();
	}

	inline void MTLParser::Warning(const String& message)
	{
		NazaraWarning(message + " at line #" + String::Number(m_lineCount));
	}

	inline void MTLParser::UnrecognizedLine(bool error)
	{
		String message = "Unrecognized \"" + m_currentLine + '"';

		if (error)
			Error(message);
		else
			Warning(message);
	}
}

#include <Nazara/Utility/DebugOff.hpp>

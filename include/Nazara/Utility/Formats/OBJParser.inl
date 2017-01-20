// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/OBJParser.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline void OBJParser::Clear()
	{
		m_materials.clear();
		m_meshes.clear();
		m_positions.clear();
		m_normals.clear();
		m_texCoords.clear();
	}

	inline String* OBJParser::GetMaterials()
	{
		return m_materials.data();
	}

	inline const String* OBJParser::GetMaterials() const
	{
		return m_materials.data();
	}

	inline UInt32 OBJParser::GetMaterialCount() const
	{
		return static_cast<UInt32>(m_materials.size());
	}

	inline OBJParser::Mesh* OBJParser::GetMeshes()
	{
		return m_meshes.data();
	}

	inline const OBJParser::Mesh* OBJParser::GetMeshes() const
	{
		return m_meshes.data();
	}

	inline UInt32 OBJParser::GetMeshCount() const
	{
		return static_cast<UInt32>(m_meshes.size());
	}

	inline const String& OBJParser::GetMtlLib() const
	{
		return m_mtlLib;
	}

	inline Vector3f* OBJParser::GetNormals()
	{
		return m_normals.data();
	}

	inline const Vector3f* OBJParser::GetNormals() const
	{
		return m_normals.data();
	}

	inline UInt32 OBJParser::GetNormalCount() const
	{
		return static_cast<UInt32>(m_normals.size());
	}

	inline Vector4f* OBJParser::GetPositions()
	{
		return m_positions.data();
	}

	inline const Vector4f* OBJParser::GetPositions() const
	{
		return m_positions.data();
	}

	inline UInt32 OBJParser::GetPositionCount() const
	{
		return static_cast<UInt32>(m_positions.size());
	}

	inline Vector3f* OBJParser::GetTexCoords()
	{
		return m_texCoords.data();
	}

	inline const Vector3f* OBJParser::GetTexCoords() const
	{
		return m_texCoords.data();
	}

	inline UInt32 OBJParser::GetTexCoordCount() const
	{
		return static_cast<UInt32>(m_texCoords.size());
	}

	inline String* OBJParser::SetMaterialCount(UInt32 materialCount)
	{
		m_materials.resize(materialCount);
		return m_materials.data();
	}

	inline OBJParser::Mesh* OBJParser::SetMeshCount(UInt32 meshCount)
	{
		m_meshes.resize(meshCount);
		return m_meshes.data();
	}

	inline void OBJParser::SetMtlLib(const String& mtlLib)
	{
		m_mtlLib = mtlLib;
	}

	inline Vector3f* OBJParser::SetNormalCount(UInt32 normalCount)
	{
		m_normals.resize(normalCount);
		return m_normals.data();
	}

	inline Vector4f* OBJParser::SetPositionCount(UInt32 positionCount)
	{
		m_positions.resize(positionCount);
		return m_positions.data();
	}

	inline Vector3f* OBJParser::SetTexCoordCount(UInt32 texCoordCount)
	{
		m_texCoords.resize(texCoordCount);
		return m_texCoords.data();
	}

	template<typename T>
	void OBJParser::Emit(const T& text) const
	{
		m_outputStream << text;
		if (m_outputStream.GetBufferSize() > 1024 * 1024)
			Flush();
	}

	inline void OBJParser::EmitLine() const
	{
		Emit('\n');
	}

	template<typename T>
	void OBJParser::EmitLine(const T& line) const
	{
		Emit(line);
		Emit('\n');
	}

	inline void OBJParser::Error(const String& message)
	{
		NazaraError(message + " at line #" + String::Number(m_lineCount));
	}

	inline void OBJParser::Flush() const
	{
		m_currentStream->Write(m_outputStream);
		m_outputStream.Clear();
	}

	inline void OBJParser::Warning(const String& message)
	{
		NazaraWarning(message + " at line #" + String::Number(m_lineCount));
	}

	inline bool OBJParser::UnrecognizedLine(bool error)
	{
		String message = "Unrecognized \"" + m_currentLine + '"';

		if (error)
			Error(message);
		else
			Warning(message);

		m_errorCount++;

		if (m_errorCount > 10 && (m_errorCount * 100 / m_lineCount) > 50)
		{
			NazaraError("Aborting parsing because of error percentage");
			return false; //< Abort parsing if error percentage is too high
		}

		return true;
	}
}

#include <Nazara/Utility/DebugOff.hpp>

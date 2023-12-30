// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

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

	inline std::string* OBJParser::GetMaterials()
	{
		return m_materials.data();
	}

	inline const std::string* OBJParser::GetMaterials() const
	{
		return m_materials.data();
	}

	inline std::size_t OBJParser::GetMaterialCount() const
	{
		return m_materials.size();
	}

	inline OBJParser::Mesh* OBJParser::GetMeshes()
	{
		return m_meshes.data();
	}

	inline const OBJParser::Mesh* OBJParser::GetMeshes() const
	{
		return m_meshes.data();
	}

	inline std::size_t OBJParser::GetMeshCount() const
	{
		return m_meshes.size();
	}

	inline const std::filesystem::path& OBJParser::GetMtlLib() const
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

	inline std::size_t OBJParser::GetNormalCount() const
	{
		return m_normals.size();
	}

	inline Vector4f* OBJParser::GetPositions()
	{
		return m_positions.data();
	}

	inline const Vector4f* OBJParser::GetPositions() const
	{
		return m_positions.data();
	}

	inline std::size_t OBJParser::GetPositionCount() const
	{
		return m_positions.size();
	}

	inline Vector3f* OBJParser::GetTexCoords()
	{
		return m_texCoords.data();
	}

	inline const Vector3f* OBJParser::GetTexCoords() const
	{
		return m_texCoords.data();
	}

	inline std::size_t OBJParser::GetTexCoordCount() const
	{
		return m_texCoords.size();
	}

	inline std::string* OBJParser::SetMaterialCount(std::size_t materialCount)
	{
		m_materials.resize(materialCount);
		return m_materials.data();
	}

	inline OBJParser::Mesh* OBJParser::SetMeshCount(std::size_t meshCount)
	{
		m_meshes.resize(meshCount);
		return m_meshes.data();
	}

	inline void OBJParser::SetMtlLib(const std::filesystem::path& mtlLib)
	{
		m_mtlLib = mtlLib;
	}

	inline Vector3f* OBJParser::SetNormalCount(std::size_t normalCount)
	{
		m_normals.resize(normalCount);
		return m_normals.data();
	}

	inline Vector4f* OBJParser::SetPositionCount(std::size_t positionCount)
	{
		m_positions.resize(positionCount);
		return m_positions.data();
	}

	inline Vector3f* OBJParser::SetTexCoordCount(std::size_t texCoordCount)
	{
		m_texCoords.resize(texCoordCount);
		return m_texCoords.data();
	}

	template<typename T>
	void OBJParser::Emit(const T& text) const
	{
		m_outputStream << text;
		if (m_outputStream.rdbuf()->str().size() > 1024 * 1024)
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

	inline void OBJParser::Error(std::string_view message)
	{
		NazaraErrorFmt("{0} on line #{1}", message, m_lineCount);
	}

	inline void OBJParser::Flush() const
	{
		m_currentStream->Write(std::move(m_outputStream).str());
		m_outputStream.str({});
	}

	inline void OBJParser::Warning(std::string_view message)
	{
		NazaraWarningFmt("{0} on line #{1}", message, m_lineCount);
	}

	inline bool OBJParser::UnrecognizedLine(bool error)
	{
		std::string message = "Unrecognized \"" + m_currentLine + '"';

		if (error)
			Error(message);
		else
			Warning(message);

		m_errorCount++;

		if (m_errorCount > 10 && (m_errorCount * 100 / m_lineCount) > 50)
		{
			NazaraError("aborting parsing because of error percentage");
			return false; //< Abort parsing if error percentage is too high
		}

		return true;
	}
}

#include <Nazara/Utility/DebugOff.hpp>

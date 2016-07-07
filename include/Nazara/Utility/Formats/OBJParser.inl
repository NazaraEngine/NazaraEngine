// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/OBJParser.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline const String* OBJParser::GetMaterials() const
	{
		return m_materials.data();
	}

	inline unsigned int OBJParser::GetMaterialCount() const
	{
		return m_materials.size();
	}

	inline const OBJParser::Mesh* OBJParser::GetMeshes() const
	{
		return m_meshes.data();
	}

	inline unsigned int OBJParser::GetMeshCount() const
	{
		return m_meshes.size();
	}

	inline const String& OBJParser::GetMtlLib() const
	{
		return m_mtlLib;
	}

	inline const Vector3f* OBJParser::GetNormals() const
	{
		return m_normals.data();
	}

	inline unsigned int OBJParser::GetNormalCount() const
	{
		return m_normals.size();
	}

	inline const Vector4f* OBJParser::GetPositions() const
	{
		return m_positions.data();
	}

	inline unsigned int OBJParser::GetPositionCount() const
	{
		return m_positions.size();
	}

	inline const Vector3f* OBJParser::GetTexCoords() const
	{
		return m_texCoords.data();
	}

	inline unsigned int OBJParser::GetTexCoordCount() const
	{
		return m_texCoords.size();
	}


	inline void OBJParser::Error(const String& message)
	{
		NazaraError(message + " at line #" + String::Number(m_lineCount));
	}

	inline void OBJParser::Warning(const String& message)
	{
		NazaraWarning(message + " at line #" + String::Number(m_lineCount));
	}

	inline void OBJParser::UnrecognizedLine(bool error)
	{
		String message = "Unrecognized \"" + m_currentLine + '"';

		if (error)
			Error(message);
		else
			Warning(message);
	}
}

#include <Nazara/Utility/DebugOff.hpp>

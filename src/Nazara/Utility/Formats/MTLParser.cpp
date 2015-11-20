// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/MTLParser.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Utility/Config.hpp>
#include <cstdio>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	MTLParser::MTLParser(Stream& stream) :
	m_stream(stream),
	m_streamFlags(stream.GetStreamOptions()) //< Saves stream flags
	{
		m_stream.EnableTextMode(true);
	}

	MTLParser::~MTLParser()
	{
		// Reset stream flags
		if ((m_streamFlags & StreamOption_Text) == 0)
			m_stream.EnableTextMode(false);
	}

	const MTLParser::Material* MTLParser::GetMaterial(const String& materialName) const
	{
		auto it = m_materials.find(materialName);
		if (it != m_materials.end())
			return &it->second;
		else
			return nullptr;
	}

	const std::unordered_map<String, MTLParser::Material>& MTLParser::GetMaterials() const
	{
		return m_materials;
	}

	bool MTLParser::Parse()
	{
		m_keepLastLine = false;
		m_lineCount = 0;
		m_materials.clear();

		Material* currentMaterial = nullptr;

		while (Advance(false))
		{
			String keyword = m_currentLine.GetWord(0).ToLower();
			if (keyword == "ka")
			{
				float r, g, b;
				if (std::sscanf(&m_currentLine[3], "%f %f %f", &r, &g, &b) == 3)
				{
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->ambient = Color(static_cast<UInt8>(r*255.f), static_cast<UInt8>(g*255.f), static_cast<UInt8>(b*255.f));
				}
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				else
					UnrecognizedLine();
				#endif
			}
			else if (keyword == "kd")
			{
				float r, g, b;
				if (std::sscanf(&m_currentLine[3], "%f %f %f", &r, &g, &b) == 3)
				{
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->diffuse = Color(static_cast<UInt8>(r*255.f), static_cast<UInt8>(g*255.f), static_cast<UInt8>(b*255.f));
				}
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				else
					UnrecognizedLine();
				#endif
			}
			else if (keyword == "ks")
			{
				float r, g, b;
				if (std::sscanf(&m_currentLine[3], "%f %f %f", &r, &g, &b) == 3)
				{
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->specular = Color(static_cast<UInt8>(r*255.f), static_cast<UInt8>(g*255.f), static_cast<UInt8>(b*255.f));
				}
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				else
					UnrecognizedLine();
				#endif
			}
			else if (keyword == "ni")
			{
				float density;
				if (std::sscanf(&m_currentLine[3], "%f", &density) == 1)
				{
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->refractionIndex = density;
				}
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				else
					UnrecognizedLine();
				#endif
			}
			else if (keyword == "ns")
			{
				float coef;
				if (std::sscanf(&m_currentLine[3], "%f", &coef) == 1)
				{
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->shininess = coef;
				}
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				else
					UnrecognizedLine();
				#endif
			}
			else if (keyword == 'd')
			{
				float alpha;
				if (std::sscanf(&m_currentLine[(keyword[0] == 'd') ? 2 : 3], "%f", &alpha) == 1)
				{
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->alpha = alpha;
				}
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				else
					UnrecognizedLine();
				#endif
			}
			else if (keyword == "tr")
			{
				float alpha;
				if (std::sscanf(&m_currentLine[(keyword[0] == 'd') ? 2 : 3], "%f", &alpha) == 1)
				{
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->alpha = 1.f - alpha; // tr vaut pour la "valeur de transparence", 0 = opaque
				}
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				else
					UnrecognizedLine();
				#endif
			}
			else if (keyword == "illum")
			{
				unsigned int model;
				if (std::sscanf(&m_currentLine[6], "%u", &model) == 1)
				{
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->illumModel = model;
				}
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				else
					UnrecognizedLine();
				#endif
			}
			else if (keyword == "map_ka")
			{
				unsigned int mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->ambientMap = map;
				}
			}
			else if (keyword == "map_kd")
			{
				unsigned int mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->diffuseMap = map;
				}
			}
			else if (keyword == "map_ks")
			{
				unsigned int mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->specularMap = map;
				}
			}
			else if (keyword == "map_bump" || keyword == "bump")
			{
				unsigned int mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->bumpMap = map;
				}
			}
			else if (keyword == "map_d")
			{
				unsigned int mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->alphaMap = map;
				}
			}
			else if (keyword == "map_decal" || keyword == "decal")
			{
				unsigned int mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->decalMap = map;
				}
			}
			else if (keyword == "map_disp" || keyword == "disp")
			{
				unsigned int mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->displacementMap = map;
				}
			}
			else if (keyword == "map_refl" || keyword == "refl")
			{
				unsigned int mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = &m_materials["default"];

					currentMaterial->reflectionMap = map;
				}
			}
			else if (keyword == "newmtl")
			{
				String materialName = m_currentLine.SubString(m_currentLine.GetWordPosition(1));
				if (!materialName.IsEmpty())
					currentMaterial = &m_materials[materialName];
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				else
					UnrecognizedLine();
				#endif
			}
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			else
				UnrecognizedLine();
			#endif
		}

		return true;
	}

	bool MTLParser::Advance(bool required)
	{
		if (!m_keepLastLine)
		{
			do
			{
				if (m_stream.EndOfStream())
				{
					if (required)
						Error("Incomplete MTL file");

					return false;
				}

				m_lineCount++;

				m_currentLine = m_stream.ReadLine();
				m_currentLine = m_currentLine.SubStringTo("#"); // On ignore les commentaires
				m_currentLine.Simplify(); // Pour un traitement plus simple
			}
			while (m_currentLine.IsEmpty());
		}
		else
			m_keepLastLine = false;

		return true;
	}

	void MTLParser::Error(const String& message)
	{
		NazaraError(message + " at line #" + String::Number(m_lineCount));
	}

	void MTLParser::Warning(const String& message)
	{
		NazaraWarning(message + " at line #" + String::Number(m_lineCount));
	}

	void MTLParser::UnrecognizedLine(bool error)
	{
		String message = "Unrecognized \"" + m_currentLine + '"';

		if (error)
			Error(message);
		else
			Warning(message);
	}
}

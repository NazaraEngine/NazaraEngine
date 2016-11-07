// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/MTLParser.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Utility/Config.hpp>
#include <cstdio>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	bool MTLParser::Parse(Stream& stream)
	{
		m_currentStream = &stream;

		// Force stream in text mode, reset it at the end
		Nz::CallOnExit resetTextMode;
		if ((stream.GetStreamOptions() & StreamOption_Text) == 0)
		{
			stream.EnableTextMode(true);

			resetTextMode.Reset([&stream] ()
			{
				stream.EnableTextMode(false);
			});
		}

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
						currentMaterial = AddMaterial("default");

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
						currentMaterial = AddMaterial("default");

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
						currentMaterial = AddMaterial("default");

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
						currentMaterial = AddMaterial("default");

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
						currentMaterial = AddMaterial("default");

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
						currentMaterial = AddMaterial("default");

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
						currentMaterial = AddMaterial("default");

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
						currentMaterial = AddMaterial("default");

					currentMaterial->illumModel = model;
				}
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				else
					UnrecognizedLine();
				#endif
			}
			else if (keyword == "map_ka")
			{
				std::size_t mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = AddMaterial("default");

					currentMaterial->ambientMap = map;
				}
			}
			else if (keyword == "map_kd")
			{
				std::size_t mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = AddMaterial("default");

					currentMaterial->diffuseMap = map;
				}
			}
			else if (keyword == "map_ks")
			{
				std::size_t mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = AddMaterial("default");

					currentMaterial->specularMap = map;
				}
			}
			else if (keyword == "map_bump" || keyword == "bump")
			{
				std::size_t mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = AddMaterial("default");

					currentMaterial->bumpMap = map;
				}
			}
			else if (keyword == "map_d")
			{
				std::size_t mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = AddMaterial("default");

					currentMaterial->alphaMap = map;
				}
			}
			else if (keyword == "map_decal" || keyword == "decal")
			{
				std::size_t mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = AddMaterial("default");

					currentMaterial->decalMap = map;
				}
			}
			else if (keyword == "map_disp" || keyword == "disp")
			{
				std::size_t mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = AddMaterial("default");

					currentMaterial->displacementMap = map;
				}
			}
			else if (keyword == "map_refl" || keyword == "refl")
			{
				std::size_t mapPos = m_currentLine.GetWordPosition(1);
				if (mapPos != String::npos)
				{
					String map = m_currentLine.SubString(mapPos);
					if (!currentMaterial)
						currentMaterial = AddMaterial("default");

					currentMaterial->reflectionMap = map;
				}
			}
			else if (keyword == "newmtl")
			{
				String materialName = m_currentLine.SubString(m_currentLine.GetWordPosition(1));
				if (!materialName.IsEmpty())
					currentMaterial = AddMaterial(materialName);
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

	bool MTLParser::Save(Stream& stream) const
	{
		m_currentStream = &stream;

		// Force stream in text mode, reset it at the end
		Nz::CallOnExit resetTextMode;
		if ((stream.GetStreamOptions() & StreamOption_Text) == 0)
		{
			stream.EnableTextMode(true);

			resetTextMode.Reset([&stream] ()
			{
				stream.EnableTextMode(false);
			});
		}

		m_outputStream.Clear();

		EmitLine("# Exported by Nazara Engine");
		EmitLine();

		Emit("# material count: ");
		Emit(m_materials.size());
		EmitLine();

		for (auto& pair : m_materials)
		{
			const String& matName = pair.first;
			const Material& mat = pair.second;

			Emit("newmtl ");
			EmitLine(matName);
			EmitLine();

			Emit("Ka ");
			Emit(mat.ambient.r / 255.f);
			Emit(' ');
			Emit(mat.ambient.g / 255.f);
			Emit(' ');
			Emit(mat.ambient.b / 255.f);
			EmitLine();

			Emit("Kd ");
			Emit(mat.diffuse.r / 255.f);
			Emit(' ');
			Emit(mat.diffuse.g / 255.f);
			Emit(' ');
			Emit(mat.diffuse.b / 255.f);
			EmitLine();

			Emit("Ks ");
			Emit(mat.specular.r / 255.f);
			Emit(' ');
			Emit(mat.specular.g / 255.f);
			Emit(' ');
			Emit(mat.specular.b / 255.f);
			EmitLine();

			if (!NumberEquals(mat.alpha, 1.f))
			{
				Emit("d ");
				EmitLine(mat.alpha);
			}

			if (!NumberEquals(mat.refractionIndex, 1.f))
			{
				Emit("ni ");
				EmitLine(mat.refractionIndex);
			}

			if (!NumberEquals(mat.shininess, 1.f))
			{
				Emit("ns ");
				EmitLine(mat.shininess);
			}

			if (mat.illumModel != 0)
			{
				Emit("illum ");
				EmitLine(mat.illumModel);
			}

			if (!mat.ambientMap.IsEmpty())
			{
				Emit("map_Ka ");
				EmitLine(mat.ambientMap);
			}

			if (!mat.diffuseMap.IsEmpty())
			{
				Emit("map_Kd ");
				EmitLine(mat.diffuseMap);
			}

			if (!mat.specularMap.IsEmpty())
			{
				Emit("map_Ks ");
				EmitLine(mat.specularMap);
			}

			if (!mat.bumpMap.IsEmpty())
			{
				Emit("map_bump ");
				EmitLine(mat.bumpMap);
			}

			if (!mat.alphaMap.IsEmpty())
			{
				Emit("map_d ");
				EmitLine(mat.alphaMap);
			}

			if (!mat.decalMap.IsEmpty())
			{
				Emit("map_decal ");
				EmitLine(mat.decalMap);
			}

			if (!mat.displacementMap.IsEmpty())
			{
				Emit("map_disp ");
				EmitLine(mat.displacementMap);
			}

			if (!mat.reflectionMap.IsEmpty())
			{
				Emit("map_refl ");
				EmitLine(mat.reflectionMap);
			}
			EmitLine();
		}

		Flush();

		return true;
	}

	bool MTLParser::Advance(bool required)
	{
		if (!m_keepLastLine)
		{
			do
			{
				if (m_currentStream->EndOfStream())
				{
					if (required)
						Error("Incomplete MTL file");

					return false;
				}

				m_lineCount++;

				m_currentLine = m_currentStream->ReadLine();
				m_currentLine = m_currentLine.SubStringTo("#"); // On ignore les commentaires
				m_currentLine.Simplify(); // Pour un traitement plus simple
			}
			while (m_currentLine.IsEmpty());
		}
		else
			m_keepLastLine = false;

		return true;
	}
}

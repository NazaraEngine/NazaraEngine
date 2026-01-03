// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Formats/MTLParser.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <cstdio>

#define NAZARA_CORE_STRICT_RESOURCE_PARSING 1

namespace Nz
{
	namespace
	{
		template<std::size_t N>
		bool TestKeyword(std::string_view currentLine, const char(&keyword)[N], std::size_t& offset)
		{
			if (currentLine.size() > N && StartsWith(currentLine, keyword, CaseIndependent{}) && std::isspace(currentLine[N - 1]))
			{
				offset = N;
				while (offset < currentLine.size() && std::isspace(currentLine[offset]))
					offset++;

				return offset < currentLine.size();
			}
			else
				return false;
		}
	}

	bool MTLParser::Parse(Stream& stream)
	{
		m_currentStream = &stream;

		// force stream in text mode, reset it at the end
		CallOnExit resetTextMode([&stream]
		{
			stream.EnableTextMode(false);
		});

		if ((stream.GetStreamOptions() & StreamOption::Text) == 0)
			stream.EnableTextMode(true);
		else
			resetTextMode.Reset();

		m_keepLastLine = false;
		m_lineCount = 0;
		m_materials.clear();

		Material* currentMaterial = nullptr;
		std::size_t offset;

		while (Advance(false))
		{
			switch (std::tolower(m_currentLine[0]))
			{
				case 'b':
				{
					if (TestKeyword(m_currentLine, "bump", offset))
					{
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->bumpMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					break;
				}

				case 'd':
				{
					if (TestKeyword(m_currentLine, "d", offset))
					{
						float alpha;
						if (std::sscanf(&m_currentLine[2], "%f", &alpha) == 1)
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->alpha = alpha;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "decal", offset))
					{
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->decalMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "disp", offset))
					{
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->displacementMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					break;
				}

				case 'e':
				{
					if (TestKeyword(m_currentLine, "emissive", offset))
					{
						// <!> This is a custom keyword
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->emissiveMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					break;
				}

				case 'k':
				{
					if (TestKeyword(m_currentLine, "ka", offset))
					{
						float r, g, b;
						if (std::sscanf(&m_currentLine[offset], "%f %f %f", &r, &g, &b) == 3)
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->ambient = Color(r, g, b);
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "kd", offset))
					{
						float r, g, b;
						if (std::sscanf(&m_currentLine[offset], "%f %f %f", &r, &g, &b) == 3)
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->diffuse = Color(r, g, b);
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "ks", offset))
					{
						float r, g, b;
						if (std::sscanf(&m_currentLine[offset], "%f %f %f", &r, &g, &b) == 3)
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->specular = Color(r, g, b);
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}

					break;
				}

				case 'i':
				{
					if (TestKeyword(m_currentLine, "illum", offset))
					{
						unsigned int model;
						if (std::sscanf(&m_currentLine[offset], "%u", &model) == 1)
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->illumModel = model;
						}
			#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
			#endif
					}
					break;
				}

				case 'm':
				{
					if (TestKeyword(m_currentLine, "map_ka", offset))
					{
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->ambientMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "map_kd", offset))
					{
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->diffuseMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "map_ks", offset))
					{
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->specularMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "map_bump", offset))
					{
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->bumpMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
					else
						UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "map_d", offset))
					{
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->alphaMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "map_decal", offset))
					{
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->decalMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "map_disp", offset))
					{
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->displacementMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "map_refl", offset))
					{
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->reflectionMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "map_normal", offset))
					{
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->normalMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "map_emissive", offset))
					{
						// <!> This is a custom keyword
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->emissiveMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					break;
				}

				case 'n':
				{
					if (TestKeyword(m_currentLine, "ni", offset))
					{
						float density;
						if (std::sscanf(&m_currentLine[offset], "%f", &density) == 1)
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->refractionIndex = density;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "ns", offset))
					{
						float coef;
						if (std::sscanf(&m_currentLine[offset], "%f", &coef) == 1)
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->shininess = coef;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "normal", offset))
					{
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->normalMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					else if (TestKeyword(m_currentLine, "newmtl", offset))
					{
						std::string materialName = m_currentLine.substr(offset);
						if (!materialName.empty())
							currentMaterial = AddMaterial(materialName);
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					break;
				}

				case 'r':
				{
					if (TestKeyword(m_currentLine, "refl", offset))
					{
						std::string map = m_currentLine.substr(offset);
						if (!map.empty())
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->reflectionMap = map;
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					break;
				}

				case 't':
				{
					if (TestKeyword(m_currentLine, "tr", offset))
					{
						float alpha;
						if (std::sscanf(&m_currentLine[offset], "%f", &alpha) == 1)
						{
							if (!currentMaterial)
								currentMaterial = AddMaterial("default");

							currentMaterial->alpha = 1.f - alpha; // tr vaut pour la "valeur de transparence", 0 = opaque
						}
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
#endif
					}
					break;
				}

				default:
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
					UnrecognizedLine();
#endif
					break;
			}
		}

		return true;
	}

	bool MTLParser::Save(Stream& stream) const
	{
		m_currentStream = &stream;

		// force stream in text mode, reset it at the end
		CallOnExit resetTextMode([&stream]
		{
			stream.EnableTextMode(false);
		});

		if ((stream.GetStreamOptions() & StreamOption::Text) == 0)
			stream.EnableTextMode(true);
		else
			resetTextMode.Reset();

		m_outputStream.str({});

		EmitLine("# Exported by Nazara Engine");
		EmitLine();

		Emit("# material count: ");
		Emit(m_materials.size());
		EmitLine();

		for (auto& pair : m_materials)
		{
			const std::string& matName = pair.first;
			const Material& mat = pair.second;

			Emit("newmtl ");
			EmitLine(matName);
			EmitLine();

			Emit("Ka ");
			Emit(mat.ambient.r);
			Emit(' ');
			Emit(mat.ambient.g);
			Emit(' ');
			Emit(mat.ambient.b);
			EmitLine();

			Emit("Kd ");
			Emit(mat.diffuse.r);
			Emit(' ');
			Emit(mat.diffuse.g);
			Emit(' ');
			Emit(mat.diffuse.b);
			EmitLine();

			Emit("Ks ");
			Emit(mat.specular.r);
			Emit(' ');
			Emit(mat.specular.g);
			Emit(' ');
			Emit(mat.specular.b);
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

			if (!mat.ambientMap.empty())
			{
				Emit("map_Ka ");
				EmitLine(mat.ambientMap);
			}

			if (!mat.diffuseMap.empty())
			{
				Emit("map_Kd ");
				EmitLine(mat.diffuseMap);
			}

			if (!mat.specularMap.empty())
			{
				Emit("map_Ks ");
				EmitLine(mat.specularMap);
			}

			if (!mat.bumpMap.empty())
			{
				Emit("map_bump ");
				EmitLine(mat.bumpMap);
			}

			if (!mat.alphaMap.empty())
			{
				Emit("map_d ");
				EmitLine(mat.alphaMap);
			}

			if (!mat.decalMap.empty())
			{
				Emit("map_decal ");
				EmitLine(mat.decalMap);
			}

			if (!mat.displacementMap.empty())
			{
				Emit("map_disp ");
				EmitLine(mat.displacementMap);
			}

			if (!mat.reflectionMap.empty())
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
				if (std::size_t p = m_currentLine.find('#'); p != m_currentLine.npos)
				{
					if (p > 0)
						m_currentLine = m_currentLine.substr(0, p - 1);
					else
						m_currentLine.clear();
				}

				m_currentLine = Trim(m_currentLine);

				if (m_currentLine.empty())
					continue;
			}
			while (m_currentLine.empty());
		}
		else
			m_keepLastLine = false;

		return true;
	}
}

#undef NAZARA_CORE_STRICT_RESOURCE_PARSING

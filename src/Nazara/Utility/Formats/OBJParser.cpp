// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/OBJParser.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Utility/Config.hpp>
#include <tsl/ordered_map.h>
#include <cctype>
#include <memory>
#include <unordered_map>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	bool OBJParser::Check(Stream& stream)
	{
		m_currentStream = &stream;
		m_errorCount = 0;
		m_keepLastLine = false;
		m_lineCount = 0;

		// Force stream in text mode, reset it at the end
		Nz::CallOnExit resetTextMode;
		if ((stream.GetStreamOptions() & StreamOption::Text) == 0)
		{
			stream.EnableTextMode(true);

			resetTextMode.Reset([&stream] ()
			{
				stream.EnableTextMode(false);
			});
		}

		unsigned int failureCount = 0;
		while (Advance(false))
		{
			switch (std::tolower(m_currentLine[0]))
			{
				case '#': //< Comment
					failureCount--;
					break;

				case 'f': //< Face
				case 'g': //< Group (inside a mesh)
				case 'o': //< Object (defines a mesh)
				case 's': //< Smooth
				{
					if (m_currentLine.size() > 1 && m_currentLine[1] == ' ')
						return true;

					break;
				}

				case 'm': //< MTLLib
					if (StartsWith(m_currentLine, "mtllib "))
						return true;

					break;

				case 'u': //< Usemtl
					if (StartsWith(m_currentLine, "usemtl "))
						return true;

					break;

				case 'v': //< Position/Normal/Texcoords
				{
					if (StartsWith(m_currentLine, "v ") ||
					    StartsWith(m_currentLine, "vn ") ||
					    StartsWith(m_currentLine, "vt "))
						return true;

					break;
				}

				default:
					break;
			}

			if (++failureCount > 20U)
				return false;
		}

		return false;
	}

	bool OBJParser::Parse(Nz::Stream& stream, std::size_t reservedVertexCount)
	{
		m_currentStream = &stream;
		m_errorCount = 0;
		m_keepLastLine = false;
		m_lineCount = 0;

		// Force stream in text mode, reset it at the end
		Nz::CallOnExit resetTextMode;
		if ((stream.GetStreamOptions() & StreamOption::Text) == 0)
		{
			stream.EnableTextMode(true);

			resetTextMode.Reset([&stream] ()
			{
				stream.EnableTextMode(false);
			});
		}

		std::string matName, meshName;
		matName = meshName = "default";
		m_meshes.clear();
		m_mtlLib.clear();

		m_normals.clear();
		m_positions.clear();
		m_texCoords.clear();

		// Reserve some space for incoming vertices
		m_normals.reserve(reservedVertexCount);
		m_positions.reserve(reservedVertexCount);
		m_texCoords.reserve(reservedVertexCount);

		// Sort meshes by material and group
		using MatPair = std::pair<Mesh, unsigned int>;
		tsl::ordered_map<std::string, tsl::ordered_map<std::string, MatPair>> meshesByName;

		UInt32 faceReserve = 0;
		UInt32 vertexReserve = 0;
		unsigned int matCount = 0;
		auto GetMaterial = [&] (const std::string& mesh, const std::string& mat) -> Mesh*
		{
			auto& map = meshesByName[mesh];
			auto it = map.find(mat);
			if (it == map.end())
				it = map.insert(std::make_pair(mat, MatPair(Mesh(), matCount++))).first;

			Mesh& meshData = it.value().first;

			meshData.faces.reserve(faceReserve);
			meshData.vertices.reserve(vertexReserve);
			faceReserve = 0;
			vertexReserve = 0;

			return &meshData;
		};

		// On prépare le mesh par défaut
		Mesh* currentMesh = nullptr;

		while (Advance(false))
		{
			switch (std::tolower(m_currentLine[0]))
			{
				case '#': //< Comment
					// Some softwares write comments to gives the number of vertex/faces an importer can expect
					unsigned int data;
					if (std::sscanf(m_currentLine.data(), "# position count: %u", &data) == 1)
						m_positions.reserve(data);
					else if (std::sscanf(m_currentLine.data(), "# normal count: %u", &data) == 1)
						m_normals.reserve(data);
					else if (std::sscanf(m_currentLine.data(), "# texcoords count: %u", &data) == 1)
						m_texCoords.reserve(data);
					else if (std::sscanf(m_currentLine.data(), "# face count: %u", &data) == 1)
						faceReserve = data;
					else if (std::sscanf(m_currentLine.data(), "# vertex count: %u", &data) == 1)
						vertexReserve = data;

					break;

				case 'f': //< Face
				{
					if (m_currentLine.size() < 7) // Since we only treat triangles, this is the minimum length of a face line (f 1 2 3)
					{
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						if (!UnrecognizedLine())
							return false;
						#endif
						break;
					}

					std::size_t vertexCount = std::count(m_currentLine.begin(), m_currentLine.end(), ' ');
					if (vertexCount < 3)
					{
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						if (!UnrecognizedLine())
							return false;
						#endif
						break;
					}

					if (!currentMesh)
						currentMesh = GetMaterial(meshName, matName);

					Face face;
					face.firstVertex = currentMesh->vertices.size();
					face.vertexCount = vertexCount;

					currentMesh->vertices.resize(face.firstVertex + vertexCount, FaceVertex{0, 0, 0});

					bool error = false;
					unsigned int pos = 2;
					for (unsigned int i = 0; i < vertexCount; ++i)
					{
						int offset;
						int n = 0;
						int p = 0;
						int t = 0;

						if (std::sscanf(&m_currentLine[pos], "%d/%d/%d%n", &p, &t, &n, &offset) != 3)
						{
							if (std::sscanf(&m_currentLine[pos], "%d//%d%n", &p, &n, &offset) != 2)
							{
								if (std::sscanf(&m_currentLine[pos], "%d/%d%n", &p, &t, &offset) != 2)
								{
									if (std::sscanf(&m_currentLine[pos], "%d%n", &p, &offset) != 1)
									{
										#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
										if (!UnrecognizedLine())
											return false;
										#endif
										error = true;
										break;
									}
								}
							}
						}

						if (p < 0)
						{
							p += static_cast<int>(m_positions.size());
							if (p < 0)
							{
								Error("Vertex index out of range (" + std::to_string(p) + " < 0");
								error = true;
								break;
							}

							++p;
						}

						if (n < 0)
						{
							n += static_cast<int>(m_normals.size());
							if (n < 0)
							{
								Error("Normal index out of range (" + std::to_string(n) + " < 0");
								error = true;
								break;
							}

							++n;
						}

						if (t < 0)
						{
							t += static_cast<int>(m_texCoords.size());
							if (t < 0)
							{
								Error("Texture coordinates index out of range (" + std::to_string(t) + " < 0");
								error = true;
								break;
							}

							++t;
						}

						if (static_cast<std::size_t>(p) > m_positions.size())
						{
							Error("Vertex index out of range (" + std::to_string(p) + " >= " + std::to_string(m_positions.size()) + ')');
							error = true;
							break;
						}
						else if (n != 0 && static_cast<std::size_t>(n) > m_normals.size())
						{
							Error("Normal index out of range (" + std::to_string(n) + " >= " + std::to_string(m_normals.size()) + ')');
							error = true;
							break;
						}
						else if (t != 0 && static_cast<std::size_t>(t) > m_texCoords.size())
						{
							Error("TexCoord index out of range (" + std::to_string(t) + " >= " + std::to_string(m_texCoords.size()) + ')');
							error = true;
							break;
						}

						currentMesh->vertices[face.firstVertex + i].normal = static_cast<UInt32>(n);
						currentMesh->vertices[face.firstVertex + i].position = static_cast<UInt32>(p);
						currentMesh->vertices[face.firstVertex + i].texCoord = static_cast<UInt32>(t);

						pos += offset;
					}

					if (!error)
						currentMesh->faces.push_back(std::move(face));
					else
						currentMesh->vertices.resize(face.firstVertex); //< Remove vertices

					break;
				}

				case 'm': //< MTLLib
				{
					const char prefix[] = "mtllib ";
					if (!StartsWith(m_currentLine, prefix))
					{
#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						if (!UnrecognizedLine())
							return false;
#endif

						break;
					}

					m_mtlLib = m_currentLine.substr(sizeof(prefix) - 1);
					break;
				}

				case 'g': //< Group (inside a mesh)
				case 'o': //< Object (defines a mesh)
				{
					if (m_currentLine.size() <= 2 || m_currentLine[1] != ' ')
					{
#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						if (!UnrecognizedLine())
							return false;
#endif
						break;
					}

					std::string objectName = m_currentLine.substr(2);
					if (objectName.empty())
					{
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						if (!UnrecognizedLine())
							return false;
						#endif
						break;
					}

					meshName = objectName;
					currentMesh = nullptr;
					break;
				}

#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				case 's': //< Smooth
					if (m_currentLine.size() <= 2 || m_currentLine[1] == ' ')
					{
						std::string param = m_currentLine.substr(2);
						if (param != "all" && param != "on" && param != "off" && !IsNumber(param))
						{
							if (!UnrecognizedLine())
								return false;
						}
					}
					else if (!UnrecognizedLine())
						return false;
					break;
#endif

				case 'u': //< Usemtl
				{
					const char prefix[] = "usemtl ";
					if (!StartsWith(m_currentLine, prefix))
					{
#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						if (!UnrecognizedLine())
							return false;
#endif

						break;
					}

					std::string newMatName = m_currentLine.substr(sizeof(prefix) - 1);
					if (newMatName.empty())
					{
#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						if (!UnrecognizedLine())
							return false;
#endif
						break;
					}

					matName = std::move(newMatName);
					currentMesh = nullptr;
					break;
				}

				case 'v': //< Position/Normal/Texcoords
				{
					if (m_currentLine.size() < 7)
					{
#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						if (!UnrecognizedLine())
							return false;
#endif
						break;
					}

					if (std::isspace(m_currentLine[1]))
					{
						Vector4f vertex(Vector3f::Zero(), 1.f);
						unsigned int paramCount = std::sscanf(&m_currentLine[2], " %f %f %f %f", &vertex.x, &vertex.y, &vertex.z, &vertex.w);
						if (paramCount >= 1)
							m_positions.push_back(vertex);
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						else if (!UnrecognizedLine())
							return false;
						#endif
					}
					else if (m_currentLine[1] == 'n' && std::isspace(m_currentLine[2]))
					{
						Vector3f normal(Vector3f::Zero());
						unsigned int paramCount = std::sscanf(&m_currentLine[3], " %f %f %f", &normal.x, &normal.y, &normal.z);
						if (paramCount == 3)
							m_normals.push_back(normal);
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						else if (!UnrecognizedLine())
							return false;
						#endif
					}
					else if (m_currentLine[1] == 't' && std::isspace(m_currentLine[2]))
					{
						Vector3f uvw(Vector3f::Zero());
						unsigned int paramCount = std::sscanf(&m_currentLine[3], " %f %f %f", &uvw.x, &uvw.y, &uvw.z);
						if (paramCount >= 2)
							m_texCoords.push_back(uvw);
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						else if (!UnrecognizedLine())
							return false;
						#endif
					}
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					else if (!UnrecognizedLine())
						return false;
					#endif

					break;
				}

				default:
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					if (!UnrecognizedLine())
						return false;
					#endif
					break;
			}
		}

		std::unordered_map<std::string, unsigned int> materials;
		m_materials.resize(matCount);

		for (auto meshIt = meshesByName.begin(); meshIt != meshesByName.end(); ++meshIt)
		{
			auto& matMap = meshIt.value();
			for (auto matIt = matMap.begin(); matIt != matMap.end(); ++matIt)
			{
				MatPair& matPair = matIt.value();
				Mesh& mesh = matPair.first;
				unsigned int index = matPair.second;

				if (!mesh.faces.empty())
				{
					const std::string& matKey = matIt.key();
					mesh.name = meshIt.key();

					auto it = materials.find(matKey);
					if (it == materials.end())
					{
						mesh.material = index;
						materials[matKey] = index;
						m_materials[index] = matKey;
					}
					else
						mesh.material = it->second;

					m_meshes.emplace_back(std::move(mesh));
				}
			}
		}

		if (m_meshes.empty())
		{
			NazaraError("No meshes");
			return false;
		}

		return true;
	}

	bool OBJParser::Save(Stream& stream) const
	{
		m_currentStream = &stream;

		// Force stream in text mode, reset it at the end
		Nz::CallOnExit resetTextMode;
		if ((stream.GetStreamOptions() & StreamOption::Text) == 0)
		{
			stream.EnableTextMode(true);

			resetTextMode.Reset([&stream] ()
			{
				stream.EnableTextMode(false);
			});
		}

		m_outputStream.str({});

		EmitLine("# Exported by Nazara Engine");
		EmitLine();

		if (!m_mtlLib.empty())
		{
			Emit("mtllib ");
			EmitLine(m_mtlLib.generic_u8string());
			EmitLine();
		}

		Emit("# position count: ");
		EmitLine(m_positions.size());

		for (const Nz::Vector4f& position : m_positions)
		{
			Emit("v ");
			Emit(position.x);
			Emit(' ');
			Emit(position.y);
			Emit(' ');
			Emit(position.z);

			if (!NumberEquals(position.w, 1.f))
			{
				Emit(' ');
				Emit(position.w);
			}

			EmitLine();
		}
		EmitLine();

		Emit("# normal count: ");
		EmitLine(m_normals.size());

		for (const Nz::Vector3f& normal : m_normals)
		{
			Emit("vn ");
			Emit(normal.x);
			Emit(' ');
			Emit(normal.y);
			Emit(' ');
			Emit(normal.y);
			EmitLine();
		}
		EmitLine();

		Emit("# texcoords count: ");
		EmitLine(m_texCoords.size());

		for (const Nz::Vector3f& uvw : m_texCoords)
		{
			Emit("vt ");
			Emit(uvw.x);
			Emit(' ');
			Emit(uvw.y);
			if (NumberEquals(uvw.z, 0.f))
			{
				Emit(' ');
				Emit(uvw.z);
			}
			EmitLine();
		}
		EmitLine();

		std::unordered_map<std::size_t /* mesh */, std::vector<std::size_t> /* meshes*/> meshesByMaterials;
		std::size_t meshIndex = 0;
		for (const Mesh& mesh : m_meshes)
			meshesByMaterials[mesh.material].push_back(meshIndex++);

		for (auto& pair : meshesByMaterials)
		{
			Emit("usemtl ");
			EmitLine(m_materials[pair.first]);
			Emit("# groups count: ");
			EmitLine(pair.second.size());
			EmitLine();

			for (std::size_t index : pair.second)
			{
				const Mesh& mesh = m_meshes[index];

				Emit("g ");
				EmitLine(mesh.name);
				EmitLine();

				Emit("# face count: ");
				EmitLine(mesh.faces.size());
				Emit("# vertex count: ");
				EmitLine(mesh.vertices.size());

				for (const Face& face : mesh.faces)
				{
					Emit('f');
					for (std::size_t i = 0; i < face.vertexCount; ++i)
					{
						Emit(' ');
						const FaceVertex& faceVertex = mesh.vertices[face.firstVertex + i];
						Emit(faceVertex.position);
						if (faceVertex.texCoord != 0 || faceVertex.normal != 0)
						{
							Emit('/');
							if (faceVertex.texCoord != 0)
								Emit(faceVertex.texCoord);

							if (faceVertex.normal != 0)
							{
								Emit('/');
								Emit(faceVertex.normal);
							}
						}
					}
					EmitLine();
				}
			}
			EmitLine();
		}

		Flush();

		return true;
	}

	bool OBJParser::Advance(bool required)
	{
		if (!m_keepLastLine)
		{
			do
			{
				if (m_currentStream->EndOfStream())
				{
					if (required)
						Error("Incomplete OBJ file");

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

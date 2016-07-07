// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/OBJParser.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Utility/Config.hpp>
#include <cctype>
#include <memory>
#include <unordered_map>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	bool OBJParser::Parse(Nz::Stream& stream, std::size_t reservedVertexCount)
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

		String matName, meshName;
		matName = meshName = "default";
		m_keepLastLine = false;
		m_lineCount = 0;
		m_meshes.clear();
		m_mtlLib.Clear();

		m_normals.clear();
		m_positions.clear();
		m_texCoords.clear();

		// Reserve some space for incoming vertices
		m_normals.reserve(reservedVertexCount);
		m_positions.reserve(reservedVertexCount);
		m_texCoords.reserve(reservedVertexCount);

		// Sort meshes by material and group
		using MatPair = std::pair<Mesh, unsigned int>;
		std::unordered_map<String, std::unordered_map<String, MatPair>> meshesByName;

		std::size_t faceReserve = 0;
		std::size_t vertexReserve = 0;
		unsigned int matCount = 0;
		auto GetMaterial = [&] (const String& meshName, const String& matName) -> Mesh*
		{
			auto& map = meshesByName[meshName];
			auto it = map.find(matName);
			if (it == map.end())
				it = map.insert(std::make_pair(matName, MatPair(Mesh(), matCount++))).first;

			Mesh& mesh = it->second.first;

			mesh.faces.reserve(faceReserve);
			mesh.vertices.reserve(vertexReserve);
			faceReserve = 0;
			vertexReserve = 0;

			return &(it->second.first);
		};

		// On prépare le mesh par défaut
		Mesh* currentMesh = nullptr;

		while (Advance(false))
		{
			switch (std::tolower(m_currentLine[0]))
			{
				case '#': //< Comment
					// Some softwares write comments to gives the number of vertex/faces an importer can expect
					std::size_t data;
					if (std::sscanf(m_currentLine.GetConstBuffer(), "# position count: %zu", &data) == 1)
						m_positions.reserve(data);
					else if (std::sscanf(m_currentLine.GetConstBuffer(), "# normal count: %zu", &data) == 1)
						m_normals.reserve(data);
					else if (std::sscanf(m_currentLine.GetConstBuffer(), "# texcoords count: %zu", &data) == 1)
						m_texCoords.reserve(data);
					else if (std::sscanf(m_currentLine.GetConstBuffer(), "# face count: %zu", &data) == 1)
						faceReserve = data;
					else if (std::sscanf(m_currentLine.GetConstBuffer(), "# vertex count: %zu", &data) == 1)
						vertexReserve = data;

					break;

				case 'f': //< Face
				{
					if (m_currentLine.GetSize() < 7) // Since we only treat triangles, this is the minimum length of a face line (f 1 2 3)
					{
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						UnrecognizedLine();
						#endif
						break;
					}

					unsigned int vertexCount = m_currentLine.Count(' ');
					if (vertexCount < 3)
					{
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						UnrecognizedLine();
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
						int& n = currentMesh->vertices[face.firstVertex + i].normal;
						int& p = currentMesh->vertices[face.firstVertex + i].position;
						int& t = currentMesh->vertices[face.firstVertex + i].texCoord;

						if (std::sscanf(&m_currentLine[pos], "%d/%d/%d%n", &p, &t, &n, &offset) != 3)
						{
							if (std::sscanf(&m_currentLine[pos], "%d//%d%n", &p, &n, &offset) != 2)
							{
								if (std::sscanf(&m_currentLine[pos], "%d/%d%n", &p, &t, &offset) != 2)
								{
									if (std::sscanf(&m_currentLine[pos], "%d%n", &p, &offset) != 1)
									{
										#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
										UnrecognizedLine();
										#endif
										error = true;
										break;
									}
								}
							}
						}

						if (p < 0)
						{
							p += m_positions.size() - 1;
							if (p < 0)
							{
								Error("Vertex index out of range (" + String::Number(p) + " < 0");
								error = true;
								break;
							}
						}

						if (n < 0)
						{
							n += m_normals.size() - 1;
							if (n < 0)
							{
								Error("Normal index out of range (" + String::Number(n) + " < 0");
								error = true;
								break;
							}
						}

						if (t < 0)
						{
							t += m_texCoords.size() - 1;
							if (t < 0)
							{
								Error("Texture coordinates index out of range (" + String::Number(t) + " < 0");
								error = true;
								break;
							}
						}

						if (static_cast<std::size_t>(p) > m_positions.size())
						{
							Error("Vertex index out of range (" + String::Number(p) + " >= " + String::Number(m_positions.size()) + ')');
							error = true;
							break;
						}
						else if (n != 0 && static_cast<std::size_t>(n) > m_normals.size())
						{
							Error("Normal index out of range (" + String::Number(n) + " >= " + String::Number(m_normals.size()) + ')');
							error = true;
							break;
						}
						else if (t != 0 && static_cast<std::size_t>(t) > m_texCoords.size())
						{
							Error("TexCoord index out of range (" + String::Number(t) + " >= " + String::Number(m_texCoords.size()) + ')');
							error = true;
							break;
						}

						pos += offset;
					}

					if (!error)
						currentMesh->faces.push_back(std::move(face));
					else
						currentMesh->vertices.resize(face.firstVertex); //< Remove vertices

					break;
				}

				case 'm': //< MTLLib
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					if (m_currentLine.GetWord(0).ToLower() != "mtllib")
						UnrecognizedLine();
					#endif

					m_mtlLib = m_currentLine.SubString(m_currentLine.GetWordPosition(1));
					break;

				case 'g': //< Group (inside a mesh)
				case 'o': //< Object (defines a mesh)
				{
					if (m_currentLine.GetSize() <= 2 || m_currentLine[1] != ' ')
					{
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						UnrecognizedLine();
						#endif
						break;
					}

					String objectName = m_currentLine.SubString(m_currentLine.GetWordPosition(1));
					if (objectName.IsEmpty())
					{
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						UnrecognizedLine();
						#endif
						break;
					}

					meshName = objectName;
					currentMesh = nullptr;
					break;
				}

				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				case 's': //< Smooth
					if (m_currentLine.GetSize() <= 2 || m_currentLine[1] == ' ')
					{
						String param = m_currentLine.SubString(2);
						if (param != "all" && param != "on" && param != "off" && !param.IsNumber())
							UnrecognizedLine();
					}
					else
						UnrecognizedLine();
					break;
					#endif

				case 'u': //< Usemtl
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					if (m_currentLine.GetWord(0) != "usemtl")
						UnrecognizedLine();
					#endif

					matName = m_currentLine.SubString(m_currentLine.GetWordPosition(1));
					currentMesh = nullptr;
					if (matName.IsEmpty())
					{
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						UnrecognizedLine();
						#endif
						break;
					}
					break;

				case 'v': //< Position/Normal/Texcoords
				{
					String word = m_currentLine.GetWord(0).ToLower();
					if (word == 'v')
					{
						Vector4f vertex(Vector3f::Zero(), 1.f);
						unsigned int paramCount = std::sscanf(&m_currentLine[2], "%f %f %f %f", &vertex.x, &vertex.y, &vertex.z, &vertex.w);
						if (paramCount >= 1)
							m_positions.push_back(vertex);
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
						#endif
					}
					else if (word == "vn")
					{
						Vector3f normal(Vector3f::Zero());
						unsigned int paramCount = std::sscanf(&m_currentLine[3], "%f %f %f", &normal.x, &normal.y, &normal.z);
						if (paramCount == 3)
							m_normals.push_back(normal);
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
						#endif
					}
					else if (word == "vt")
					{
						Vector3f uvw(Vector3f::Zero());
						unsigned int paramCount = std::sscanf(&m_currentLine[3], "%f %f %f", &uvw.x, &uvw.y, &uvw.z);
						if (paramCount >= 2)
							m_texCoords.push_back(uvw);
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						else
							UnrecognizedLine();
						#endif
					}
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					else
						UnrecognizedLine();
					#endif

					break;
				}

				default:
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					UnrecognizedLine();
					#endif
					break;
			}
		}

		std::unordered_map<String, unsigned int> materials;
		m_materials.resize(matCount);

		for (auto& meshPair : meshesByName)
		{
			for (auto& matPair : meshPair.second)
			{
				Mesh& mesh = matPair.second.first;
				unsigned int index = matPair.second.second;
				if (!mesh.faces.empty())
				{
					mesh.name = meshPair.first;

					auto it = materials.find(matPair.first);
					if (it == materials.end())
					{
						mesh.material = index;
						materials[matPair.first] = index;
						m_materials[index] = matPair.first;
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

		if (!m_mtlLib.IsEmpty())
		{
			Emit("mtlib ");
			EmitLine(m_mtlLib);
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
			if (!NumberEquals(position.z, 0.f) || !NumberEquals(position.w, 1.f))
			{
				Emit(' ');
				Emit(position.z);

				if (!NumberEquals(position.w, 1.f))
				{
					Emit(' ');
					Emit(position.w);
				}
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

			for (std::size_t meshIndex : pair.second)
			{
				const Mesh& mesh = m_meshes[meshIndex];

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
				m_currentLine.Simplify(); // Simplify lines (convert multiple blanks into a single space and trims)
			}
			while (m_currentLine.IsEmpty());
		}
		else
			m_keepLastLine = false;

		return true;
	}
}

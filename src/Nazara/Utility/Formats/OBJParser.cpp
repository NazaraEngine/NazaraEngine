// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/OBJParser.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Utility/Config.hpp>
#include <cctype>
#include <memory>
#include <unordered_map>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	OBJParser::OBJParser(Stream& stream) :
	m_stream(stream),
	m_streamFlags(stream.GetStreamOptions()) //< Saves stream flags
	{
		m_stream.EnableTextMode(true);
	}

	OBJParser::~OBJParser()
	{
		// Reset stream flags
		if ((m_streamFlags & StreamOption_Text) == 0)
			m_stream.EnableTextMode(false);
	}

	const String* OBJParser::GetMaterials() const
	{
		return m_materials.data();
	}

	unsigned int OBJParser::GetMaterialCount() const
	{
		return m_materials.size();
	}

	const OBJParser::Mesh* OBJParser::GetMeshes() const
	{
		return m_meshes.data();
	}

	unsigned int OBJParser::GetMeshCount() const
	{
		return m_meshes.size();
	}

	const String& OBJParser::GetMtlLib() const
	{
		return m_mtlLib;
	}

	const Vector3f* OBJParser::GetNormals() const
	{
		return m_normals.data();
	}

	unsigned int OBJParser::GetNormalCount() const
	{
		return m_normals.size();
	}

	const Vector4f* OBJParser::GetPositions() const
	{
		return m_positions.data();
	}

	unsigned int OBJParser::GetPositionCount() const
	{
		return m_positions.size();
	}

	const Vector3f* OBJParser::GetTexCoords() const
	{
		return m_texCoords.data();
	}

	unsigned int OBJParser::GetTexCoordCount() const
	{
		return m_texCoords.size();
	}

	bool OBJParser::Parse(std::size_t reservedVertexCount)
	{
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

		// On va regrouper les meshs par nom et par matériau
		using FaceVec = std::vector<Face>;
		using MatPair = std::pair<FaceVec, unsigned int>;
		std::unordered_map<String, std::unordered_map<String, MatPair>> meshes;

		unsigned int matIndex = 0;
		auto GetMaterial = [&meshes, &matIndex] (const String& mesh, const String& material) -> FaceVec*
		{
			auto& map = meshes[mesh];
			auto it = map.find(material);
			if (it == map.end())
				it = map.insert(std::make_pair(material, MatPair(FaceVec(), matIndex++))).first;

			return &(it->second.first);
		};

		// On prépare le mesh par défaut
		FaceVec* currentMesh = nullptr;

		while (Advance(false))
		{
			switch (std::tolower(m_currentLine[0]))
			{
				case 'f': // Une face
				{
					if (m_currentLine.GetSize() < 7) // Le minimum syndical pour définir une face de trois sommets (f 1 2 3)
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

					Face face;
					face.vertices.resize(vertexCount);

					bool error = false;
					unsigned int pos = 2;
					for (unsigned int i = 0; i < vertexCount; ++i)
					{
						int offset;
						int& n = face.vertices[i].normal;
						int& p = face.vertices[i].position;
						int& t = face.vertices[i].texCoord;

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
									else
									{
										n = 0;
										t = 0;
									}
								}
								else
									n = 0;
							}
							else
								t = 0;
						}

						if (p < 0)
						{
							p += m_positions.size();
							if (p < 0)
							{
								Error("Vertex index out of range (" + String::Number(p) + " < 0");
								error = true;
								break;
							}
						}
						else
							p--;

						if (n < 0)
						{
							n += m_normals.size();
							if (n < 0)
							{
								Error("Vertex index out of range (" + String::Number(n) + " < 0");
								error = true;
								break;
							}
						}
						else
							n--;

						if (t < 0)
						{
							t += m_texCoords.size();
							if (t < 0)
							{
								Error("Vertex index out of range (" + String::Number(t) + " < 0");
								error = true;
								break;
							}
						}
						else
							t--;

						if (static_cast<unsigned int>(p) >= m_positions.size())
						{
							Error("Vertex index out of range (" + String::Number(p) + " >= " + String::Number(m_positions.size()) + ')');
							error = true;
							break;
						}
						else if (n >= 0 && static_cast<unsigned int>(n) >= m_normals.size())
						{
							Error("Normal index out of range (" + String::Number(n) + " >= " + String::Number(m_normals.size()) + ')');
							error = true;
							break;
						}
						else if (t >= 0 && static_cast<unsigned int>(t) >= m_texCoords.size())
						{
							Error("TexCoord index out of range (" + String::Number(t) + " >= " + String::Number(m_texCoords.size()) + ')');
							error = true;
							break;
						}

						pos += offset;
					}

					if (!error)
					{
						if (!currentMesh)
							currentMesh = GetMaterial(meshName, matName);

						currentMesh->push_back(std::move(face));
					}

					break;
				}

				case 'm':
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					if (m_currentLine.GetWord(0).ToLower() != "mtllib")
						UnrecognizedLine();
					#endif

					m_mtlLib = m_currentLine.SubString(m_currentLine.GetWordPosition(1));
					break;

				case 'g':
				case 'o':
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
					currentMesh = GetMaterial(meshName, matName);
					break;
				}

				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				case 's':
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

				case 'u':
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					if (m_currentLine.GetWord(0) != "usemtl")
						UnrecognizedLine();
					#endif

					matName = m_currentLine.SubString(m_currentLine.GetWordPosition(1));
					if (matName.IsEmpty())
					{
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						UnrecognizedLine();
						#endif
						break;
					}

					currentMesh = GetMaterial(meshName, matName);
					break;

				case 'v':
				{
					String word = m_currentLine.GetWord(0).ToLower();
					if (word == 'v')
					{
						Vector4f vertex(Vector3f::Zero(), 1.f);
						unsigned int paramCount = std::sscanf(&m_currentLine[2], "%f %f %f %f", &vertex.x, &vertex.y, &vertex.z, &vertex.w);
						if (paramCount >= 3)
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
		m_materials.resize(matIndex);

		for (auto& meshIt : meshes)
		{
			for (auto& matIt : meshIt.second)
			{
				auto& faceVec = matIt.second.first;
				unsigned int index = matIt.second.second;
				if (!faceVec.empty())
				{
					Mesh mesh;
					mesh.faces = std::move(faceVec);
					mesh.name = meshIt.first;

					auto it = materials.find(matIt.first);
					if (it == materials.end())
					{
						mesh.material = index;
						materials[matIt.first] = index;
						m_materials[index] = matIt.first;
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

	bool OBJParser::Advance(bool required)
	{
		if (!m_keepLastLine)
		{
			do
			{
				if (m_stream.EndOfStream())
				{
					if (required)
						Error("Incomplete OBJ file");

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

	void OBJParser::Error(const String& message)
	{
		NazaraError(message + " at line #" + String::Number(m_lineCount));
	}

	void OBJParser::Warning(const String& message)
	{
		NazaraWarning(message + " at line #" + String::Number(m_lineCount));
	}

	void OBJParser::UnrecognizedLine(bool error)
	{
		String message = "Unrecognized \"" + m_currentLine + '"';

		if (error)
			Error(message);
		else
			Warning(message);
		}
}

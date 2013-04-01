// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/3D/Loaders/OBJ/OBJParser.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Utility/Config.hpp>
#include <cstdio>
#include <map>
#include <memory>
#include <Nazara/3D/Debug.hpp>

NzOBJParser::NzOBJParser(NzInputStream& stream) :
m_stream(stream),
m_streamFlags(stream.GetStreamOptions())
{
	if ((m_streamFlags & nzStreamOption_Text) == 0)
		m_stream.SetStreamOptions(m_streamFlags | nzStreamOption_Text);
}

NzOBJParser::~NzOBJParser()
{
	if ((m_streamFlags & nzStreamOption_Text) == 0)
		m_stream.SetStreamOptions(m_streamFlags);
}

const NzString* NzOBJParser::GetMaterials() const
{
	return &m_materials[0];
}

unsigned int NzOBJParser::GetMaterialCount() const
{
	return m_materials.size();
}

const NzOBJParser::Mesh* NzOBJParser::GetMeshes() const
{
	return &m_meshes[0];
}

unsigned int NzOBJParser::GetMeshCount() const
{
	return m_meshes.size();
}

const NzString& NzOBJParser::GetMtlLib() const
{
	return m_mtlLib;
}

const NzVector3f* NzOBJParser::GetNormals() const
{
	return &m_normals[0];
}

unsigned int NzOBJParser::GetNormalCount() const
{
	return m_normals.size();
}

const NzVector4f* NzOBJParser::GetPositions() const
{
	return &m_positions[0];
}

unsigned int NzOBJParser::GetPositionCount() const
{
	return m_positions.size();
}

const NzVector3f* NzOBJParser::GetTexCoords() const
{
	return &m_texCoords[0];
}

unsigned int NzOBJParser::GetTexCoordCount() const
{
	return m_texCoords.size();
}

bool NzOBJParser::Parse()
{
	NzString matName, meshName;
	matName = meshName = "default";
	m_keepLastLine = false;
	m_lineCount = 0;
	m_meshes.clear();
	m_mtlLib.Clear();

	m_normals.clear();
	m_positions.clear();
	m_texCoords.clear();

	// Beaucoup de meshs font plus de 100 sommets, on prépare le terrain
	m_normals.reserve(100);
	m_positions.reserve(100);
	m_texCoords.reserve(100);

	std::map<NzString, std::map<NzString, std::vector<Face>>> meshes;

	std::vector<Face>* currentMesh = &meshes[meshName][matName];

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
							Error("Vertex index out of range (" + NzString::Number(p) + " < 0");
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
							Error("Vertex index out of range (" + NzString::Number(n) + " < 0");
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
							Error("Vertex index out of range (" + NzString::Number(t) + " < 0");
							error = true;
							break;
						}
					}
					else
						t--;

					if (static_cast<unsigned int>(p) >= m_positions.size())
					{
						Error("Vertex index out of range (" + NzString::Number(p) + " >= " + NzString::Number(m_positions.size()) + ')');
						error = true;
						break;
					}
					else if (n >= 0 && static_cast<unsigned int>(n) >= m_normals.size())
					{
						Error("Normal index out of range (" + NzString::Number(n) + " >= " + NzString::Number(m_normals.size()) + ')');
						error = true;
						break;
					}
					else if (t >= 0 && static_cast<unsigned int>(t) >= m_texCoords.size())
					{
						Error("TexCoord index out of range (" + NzString::Number(t) + " >= " + NzString::Number(m_texCoords.size()) + ')');
						error = true;
						break;
					}

					pos += offset;
				}

				if (!error)
					currentMesh->push_back(std::move(face));

				break;
			}

			case 'm':
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				if (m_currentLine.GetWord(0).ToLower() != "mtllib")
					UnrecognizedLine();
				#endif

				m_mtlLib = m_currentLine.Substr(m_currentLine.GetWordPosition(1));
				break;

			case 'g':
			case 'o':
			{
				if (m_currentLine[1] != ' ')
				{
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					UnrecognizedLine();
					#endif
					break;
				}

				NzString objectName = m_currentLine.Substr(m_currentLine.GetWordPosition(1));
				if (objectName.IsEmpty())
				{
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					UnrecognizedLine();
					#endif
					break;
				}

				meshName = objectName;
				currentMesh = &meshes[meshName][matName];
				break;
			}

			case 's':
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				if (m_currentLine[1] == ' ')
				{
					NzString param = m_currentLine.Substr(2);
					if (param != "all" && param != "on" && param != "off" && !param.IsNumber())
						UnrecognizedLine();
				}
				else
					UnrecognizedLine();
				#endif
				break;

			case 'u':
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				if (m_currentLine.GetWord(0) != "usemtl")
					UnrecognizedLine();
				#endif

				matName = m_currentLine.Substr(m_currentLine.GetWordPosition(1));
				if (matName.IsEmpty())
				{
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					UnrecognizedLine();
					#endif
					break;
				}

				currentMesh = &meshes[meshName][matName];
				break;

			case 'v':
			{
				NzString word = m_currentLine.GetWord(0).ToLower();
				if (word == 'v')
				{
					NzVector4f vertex(NzVector3f::Zero(), 1.f);
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
					NzVector3f normal(NzVector3f::Zero());
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
					NzVector3f uvw(NzVector3f::Zero());
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

	std::map<NzString, unsigned int> materials;
	unsigned int matCount = 0;

	for (auto meshIt : meshes)
	{
		for (auto matIt : meshIt.second)
		{
			if (!matIt.second.empty())
			{
				Mesh mesh;
				mesh.faces = std::move(matIt.second);
				mesh.name = meshIt.first;

				auto it = materials.find(matIt.first);
				if (it == materials.end())
				{
					mesh.material = matCount;
					materials[matIt.first] = matCount++;
				}
				else
					mesh.material = it->second;

				m_meshes.push_back(std::move(mesh));
			}
		}
	}

	if (m_meshes.empty())
	{
		NazaraError("No meshes");
		return false;
	}

	m_materials.resize(matCount);
	for (auto it : materials)
		m_materials[it.second] = it.first;

	return true;
}

bool NzOBJParser::Advance(bool required)
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
			m_currentLine = m_currentLine.SubstrTo("#"); // On ignore les commentaires
			m_currentLine.Simplify(); // Pour un traitement plus simple
		}
		while (m_currentLine.IsEmpty());
	}
	else
		m_keepLastLine = false;

	return true;
}

void NzOBJParser::Error(const NzString& message)
{
	NazaraError(message + " at line #" + NzString::Number(m_lineCount));
}

void NzOBJParser::Warning(const NzString& message)
{
	NazaraWarning(message + " at line #" + NzString::Number(m_lineCount));
}

void NzOBJParser::UnrecognizedLine(bool error)
{
	NzString message = "Unrecognized \"" + m_currentLine + '"';

	if (error)
		Error(message);
	else
		Warning(message);
}

// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Loaders/MD5Mesh/Parser.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/IndexIterator.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <cstdio>
#include <cstring>
#include <limits>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

NzMD5MeshParser::NzMD5MeshParser(NzInputStream& stream) :
m_stream(stream),
m_keepLastLine(false),
m_lineCount(0),
m_meshIndex(0),
m_streamFlags(stream.GetStreamOptions())
{
	if ((m_streamFlags & nzStreamOption_Text) == 0)
		m_stream.SetStreamOptions(m_streamFlags | nzStreamOption_Text);
}

NzMD5MeshParser::~NzMD5MeshParser()
{
	if ((m_streamFlags & nzStreamOption_Text) == 0)
		m_stream.SetStreamOptions(m_streamFlags);
}

nzTernary NzMD5MeshParser::Check()
{
	if (Advance(false))
	{
		unsigned int version;
		if (std::sscanf(&m_currentLine[0], " MD5Version %u", &version) == 1)
		{
			if (version == 10)
				return nzTernary_True;
		}
	}

	return nzTernary_False;
}

const NzMD5MeshParser::Joint* NzMD5MeshParser::GetJoints() const
{
	return m_joints.data();
}

unsigned int NzMD5MeshParser::GetJointCount() const
{
	return m_joints.size();
}

const NzMD5MeshParser::Mesh* NzMD5MeshParser::GetMeshes() const
{
	return m_meshes.data();
}

unsigned int NzMD5MeshParser::GetMeshCount() const
{
	return m_meshes.size();
}

bool NzMD5MeshParser::Parse()
{
	while (Advance(false))
	{
		switch (m_currentLine[0])
		{
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			case 'M': // MD5Version
				if (m_currentLine.GetWord(0) != "MD5Version")
					UnrecognizedLine();
				break;

			case 'c': // commandline
				if (m_currentLine.GetWord(0) != "commandline")
					UnrecognizedLine();
				break;
			#endif

			case 'j': // joints
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				if (!m_currentLine.StartsWith("joints {"))
				{
					UnrecognizedLine();
					break;
				}
				#endif

				if (!ParseJoints())
				{
					Error("Failed to parse joints");
					return false;
				}
				break;

			case 'm': // mesh
			{
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				if (m_currentLine != "mesh {")
				{
					UnrecognizedLine();
					break;
				}
				#endif

				if (m_meshIndex >= m_meshes.size())
				{
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					Warning("More meshes than registred");
					#endif

					m_meshes.push_back(Mesh());
				}

				if (!ParseMesh())
				{
					NazaraError("Failed to parse mesh");
					return false;
				}

				m_meshIndex++;
				break;
			}

			case 'n': // num[Frames/Joints]
			{
				unsigned int count;
				if (std::sscanf(&m_currentLine[0], "numJoints %u", &count) == 1)
				{
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					if (!m_joints.empty())
						Warning("Joint count is already defined");
					#endif

					m_joints.resize(count);
				}
				else if (std::sscanf(&m_currentLine[0], "numMeshes %u", &count) == 1)
				{
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					if (!m_meshes.empty())
						Warning("Mesh count is already defined");
					#endif

					m_meshes.resize(count);
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

	return true;
}

bool NzMD5MeshParser::Advance(bool required)
{
	if (!m_keepLastLine)
	{
		do
		{
			if (m_stream.EndOfStream())
			{
				if (required)
					Error("Incomplete MD5 file");

				return false;
			}

			m_lineCount++;

			m_currentLine = m_stream.ReadLine();
			m_currentLine = m_currentLine.SubStringTo("//"); // On ignore les commentaires
			m_currentLine.Simplify(); // Pour un traitement plus simple
		}
		while (m_currentLine.IsEmpty());
	}
	else
		m_keepLastLine = false;

	return true;
}

void NzMD5MeshParser::Error(const NzString& message)
{
	NazaraError(message + " at line #" + NzString::Number(m_lineCount));
}

bool NzMD5MeshParser::ParseJoints()
{
	unsigned int jointCount = m_joints.size();
	if (jointCount == 0)
	{
		Error("Joint count is invalid or missing");
		return false;
	}

	for (unsigned int i = 0; i < jointCount; ++i)
	{
		if (!Advance())
			return false;

		unsigned int pos = m_currentLine.Find(' ');
		if (pos == NzString::npos)
		{
			UnrecognizedLine(true);
			return false;
		}

		if (pos >= 64)
		{
			NazaraError("Joint name is too long (>= 64 characters)");
			return false;
		}

		char name[64];
		if (std::sscanf(&m_currentLine[0], "%s %d ( %f %f %f ) ( %f %f %f )", &name[0], &m_joints[i].parent,
																			  &m_joints[i].bindPos.x, &m_joints[i].bindPos.y, &m_joints[i].bindPos.z,
																			  &m_joints[i].bindOrient.x, &m_joints[i].bindOrient.y, &m_joints[i].bindOrient.z) != 8)
		{
			UnrecognizedLine(true);
			return false;
		}

		m_joints[i].name = name;
		m_joints[i].name.Trim('"');

		int parent = m_joints[i].parent;
		if (parent >= 0)
		{
			if (static_cast<unsigned int>(parent) >= jointCount)
			{
				Error("Joint's parent is out of bounds (" + NzString::Number(parent) + " >= " + NzString::Number(jointCount) + ')');
				return false;
			}
		}

		m_joints[i].bindOrient.ComputeW(); // On calcule la composante W
	}

	if (!Advance())
		return false;

	if (m_currentLine != '}')
	{
		#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
		Warning("Hierarchy braces closing not found");
		#endif

		// On tente de survivre à l'erreur
		m_keepLastLine = true;
	}

	return true;
}

bool NzMD5MeshParser::ParseMesh()
{
	bool finished = false;
	while (!finished && Advance(false))
	{
		switch (m_currentLine[0])
		{
			case '}':
				finished = true;
				break;

			case 's': // shader
				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				if (!m_currentLine.StartsWith("shader "))
				{
					UnrecognizedLine();
					break;
				}
				#endif

				m_meshes[m_meshIndex].shader = m_currentLine.SubString(7);
				m_meshes[m_meshIndex].shader.Trim('"');
				break;

			case 'n': // num[tris/verts]
			{
				unsigned int count;
				if (std::sscanf(&m_currentLine[0], "numtris %u", &count) == 1)
				{
					m_meshes[m_meshIndex].triangles.resize(count);
					for (unsigned int i = 0; i < count; ++i)
					{
						if (!Advance())
							return false;

						Triangle& triangle = m_meshes[m_meshIndex].triangles[i];
						unsigned int index;
						if (std::sscanf(&m_currentLine[0], "tri %u %u %u %u", &index, &triangle.x, &triangle.y, &triangle.z) != 4)
						{
							UnrecognizedLine(true);
							return false;
						}

						if (index != i)
						{
							Error("Unexpected triangle index (expected " + NzString::Number(i) + ", got " + NzString::Number(index) + ')');
							return false;
						}
					}
				}
				else if (std::sscanf(&m_currentLine[0], "numverts %u", &count) == 1)
				{
					m_meshes[m_meshIndex].vertices.resize(count);
					for (unsigned int i = 0; i < count; ++i)
					{
						if (!Advance())
							return false;

						Vertex& vertex = m_meshes[m_meshIndex].vertices[i];
						unsigned int index;
						if (std::sscanf(&m_currentLine[0], "vert %u ( %f %f ) %u %u", &index, &vertex.uv.x, &vertex.uv.y, &vertex.startWeight, &vertex.weightCount) != 5)
						{
							UnrecognizedLine(true);
							return false;
						}

						if (index != i)
						{
							Error("Unexpected vertex index (expected " + NzString::Number(i) + ", got " + NzString::Number(index) + ')');
							return false;
						}
					}
				}
				else if (std::sscanf(&m_currentLine[0], "numweights %u", &count) == 1)
				{
					m_meshes[m_meshIndex].weights.resize(count);
					for (unsigned int i = 0; i < count; ++i)
					{
						if (!Advance())
							return false;

						Weight& weight = m_meshes[m_meshIndex].weights[i];
						unsigned int index;
						if (std::sscanf(&m_currentLine[0], "weight %u %u %f ( %f %f %f )", &index, &weight.joint, &weight.bias,
																						   &weight.pos.x, &weight.pos.y, &weight.pos.z) != 6)
						{
							UnrecognizedLine(true);
							return false;
						}

						if (index != i)
						{
							Error("Unexpected weight index (expected " + NzString::Number(i) + ", got " + NzString::Number(index) + ')');
							return false;
						}
					}
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

	if (m_meshes[m_meshIndex].triangles.empty())
	{
		NazaraError("Mesh has no triangles");
		return false;
	}

	if (m_meshes[m_meshIndex].vertices.empty())
	{
		NazaraError("Mesh has no vertices");
		return false;
	}

	if (m_meshes[m_meshIndex].weights.empty())
	{
		NazaraError("Mesh has no weights");
		return false;
	}

	#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
	if (!finished)
		Warning("Mesh braces closing not found");
	#endif

	return true;
}

void NzMD5MeshParser::Warning(const NzString& message)
{
	NazaraWarning(message + " at line #" + NzString::Number(m_lineCount));
}

void NzMD5MeshParser::UnrecognizedLine(bool error)
{
	NzString message = "Unrecognized \"" + m_currentLine + '"';

	if (error)
		Error(message);
	else
		Warning(message);
}

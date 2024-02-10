// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Formats/MD5MeshParser.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <cstdio>
#include <memory>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	MD5MeshParser::MD5MeshParser(Stream& stream) :
	m_stream(stream),
	m_streamFlags(stream.GetStreamOptions()), //< Saves stream flags
	m_keepLastLine(false),
	m_lineCount(0),
	m_meshIndex(0)
	{
		m_stream.EnableTextMode(true);
	}

	MD5MeshParser::~MD5MeshParser()
	{
		// Reset stream flags
		if ((m_streamFlags & StreamOption::Text) == 0)
			m_stream.EnableTextMode(false);
	}

	bool MD5MeshParser::Check()
	{
		if (Advance(false))
		{
			unsigned int version;
			if (std::sscanf(&m_currentLine[0], " MD5Version %u", &version) == 1)
			{
				if (version == 10)
					return true;
			}
		}

		return false;
	}

	const MD5MeshParser::Joint* MD5MeshParser::GetJoints() const
	{
		return m_joints.data();
	}

	UInt32 MD5MeshParser::GetJointCount() const
	{
		return static_cast<UInt32>(m_joints.size());
	}

	const MD5MeshParser::Mesh* MD5MeshParser::GetMeshes() const
	{
		return m_meshes.data();
	}

	UInt32 MD5MeshParser::GetMeshCount() const
	{
		return static_cast<UInt32>(m_meshes.size());
	}

	bool MD5MeshParser::Parse()
	{
		while (Advance(false))
		{
			switch (m_currentLine[0])
			{
				#if NAZARA_CORE_STRICT_RESOURCE_PARSING
				case 'M': // MD5Version
					if (!StartsWith(m_currentLine, "MD5Version "))
						UnrecognizedLine();
					break;

				case 'c': // commandline
					if (!StartsWith(m_currentLine, "commandline "))
						UnrecognizedLine();
					break;
				#endif

				case 'j': // joints
					#if NAZARA_CORE_STRICT_RESOURCE_PARSING
					if (!StartsWith(m_currentLine, "joints {"))
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
					#if NAZARA_CORE_STRICT_RESOURCE_PARSING
					if (!StartsWith(m_currentLine, "mesh {"))
					{
						UnrecognizedLine();
						break;
					}
					#endif

					if (m_meshIndex >= m_meshes.size())
					{
						#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						Warning("More meshes than registred");
						#endif

						m_meshes.emplace_back();
					}

					if (!ParseMesh())
					{
						NazaraError("failed to parse mesh");
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
						#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						if (!m_joints.empty())
							Warning("Joint count is already defined");
						#endif

						m_joints.resize(count);
					}
					else if (std::sscanf(&m_currentLine[0], "numMeshes %u", &count) == 1)
					{
						#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						if (!m_meshes.empty())
							Warning("Mesh count is already defined");
						#endif

						m_meshes.resize(count);
					}
					#if NAZARA_CORE_STRICT_RESOURCE_PARSING
					else
						UnrecognizedLine();
					#endif
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

	bool MD5MeshParser::Advance(bool required)
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

				if (std::size_t p = m_currentLine.find("//"); p != m_currentLine.npos)
				{
					if (p > 0)
						m_currentLine = m_currentLine.substr(0, p - 1);
					else
						m_currentLine.clear();
				}

				// Trim left
				m_currentLine.erase(m_currentLine.begin(), std::find_if(m_currentLine.begin(), m_currentLine.end(), [](char c)
				{
					return !std::isspace(c);
				}));

				if (m_currentLine.empty())
					continue;
			}
			while (m_currentLine.empty());
		}
		else
			m_keepLastLine = false;

		return true;
	}

	void MD5MeshParser::Error(std::string_view message)
	{
		NazaraErrorFmt("{0} on line #{1}", message, m_lineCount);
	}

	bool MD5MeshParser::ParseJoints()
	{
		std::size_t jointCount = m_joints.size();
		if (jointCount == 0)
		{
			Error("Joint count is invalid or missing");
			return false;
		}

		for (std::size_t i = 0; i < jointCount; ++i)
		{
			if (!Advance())
				return false;

			std::size_t pos = m_currentLine.find(' ');
			if (pos == std::string::npos)
			{
				UnrecognizedLine(true);
				return false;
			}

			if (pos >= 64)
			{
				NazaraError("joint name is too long (>= 64 characters)");
				return false;
			}

			char name[64];
			if (std::sscanf(&m_currentLine[0], "%63s %d ( %f %f %f ) ( %f %f %f )", &name[0], &m_joints[i].parent,
			                                                                        &m_joints[i].bindPos.x, &m_joints[i].bindPos.y, &m_joints[i].bindPos.z,
			                                                                        &m_joints[i].bindOrient.x, &m_joints[i].bindOrient.y, &m_joints[i].bindOrient.z) != 8)
			{
				UnrecognizedLine(true);
				return false;
			}

			m_joints[i].name = Trim(name, '"');

			Int32 parent = m_joints[i].parent;
			if (parent >= 0)
			{
				if (static_cast<std::size_t>(parent) >= jointCount)
				{
					Error("Joint's parent is out of bounds (" + std::to_string(parent) + " >= " + std::to_string(jointCount) + ')');
					return false;
				}
			}

			m_joints[i].bindOrient.ComputeW(); // On calcule la composante W
		}

		if (!Advance())
			return false;

		if (m_currentLine != "}")
		{
			#if NAZARA_CORE_STRICT_RESOURCE_PARSING
			Warning("Hierarchy braces closing not found");
			#endif

			// On tente de survivre à l'erreur
			m_keepLastLine = true;
		}

		return true;
	}

	bool MD5MeshParser::ParseMesh()
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
				{
					#if NAZARA_CORE_STRICT_RESOURCE_PARSING
					if (!StartsWith(m_currentLine, "shader "))
					{
						UnrecognizedLine();
						break;
					}
					#endif

					std::string_view shader = m_currentLine;
					shader = shader.substr(7);
					if (shader.empty())
					{
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						UnrecognizedLine();
#endif
						break;
					}

					if (shader.empty())
					{
#if NAZARA_CORE_STRICT_RESOURCE_PARSING
						UnrecognizedLine();
#endif
						break;
					}

					if (shader.front() == '"' && shader.back() == '"')
					{
						shader.remove_prefix(1);
						shader.remove_suffix(1);
					}

					m_meshes[m_meshIndex].shader = shader;
					break;
				}

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
								Error("Unexpected triangle index (expected " + std::to_string(i) + ", got " + std::to_string(index) + ')');
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
								Error("Unexpected vertex index (expected " + std::to_string(i) + ", got " + std::to_string(index) + ')');
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
								Error("Unexpected weight index (expected " + std::to_string(i) + ", got " + std::to_string(index) + ')');
								return false;
							}
						}
					}
					#if NAZARA_CORE_STRICT_RESOURCE_PARSING
					else
						UnrecognizedLine();
					#endif
					break;
				}

				default:
					#if NAZARA_CORE_STRICT_RESOURCE_PARSING
					UnrecognizedLine();
					#endif
					break;
			}
		}

		if (m_meshes[m_meshIndex].triangles.empty())
		{
			NazaraError("mesh has no triangles");
			return false;
		}

		if (m_meshes[m_meshIndex].vertices.empty())
		{
			NazaraError("mesh has no vertices");
			return false;
		}

		if (m_meshes[m_meshIndex].weights.empty())
		{
			NazaraError("mesh has no weights");
			return false;
		}

		#if NAZARA_CORE_STRICT_RESOURCE_PARSING
		if (!finished)
			Warning("Mesh braces closing not found");
		#endif

		return true;
	}

	void MD5MeshParser::Warning(std::string_view message)
	{
		NazaraWarningFmt("{0} on line #{1}", message, m_lineCount);
	}

	void MD5MeshParser::UnrecognizedLine(bool error)
	{
		std::string message = "unrecognized \"" + m_currentLine + '"';

		if (error)
			Error(message);
		else
			Warning(message);
}
}

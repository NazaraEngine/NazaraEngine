// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/MD5AnimParser.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <cstdio>
#include <cstring>
#include <limits>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	MD5AnimParser::MD5AnimParser(Stream& stream) :
	m_stream(stream),
	m_keepLastLine(false),
	m_frameIndex(0),
	m_frameRate(0),
	m_lineCount(0),
	m_streamFlags(stream.GetStreamOptions()) //< Saves stream flags
	{
		m_stream.EnableTextMode(true);
	}

	MD5AnimParser::~MD5AnimParser()
	{
		// Reset stream flags
		if ((m_streamFlags & StreamOption_Text) == 0)
			m_stream.EnableTextMode(false);
	}

	Ternary MD5AnimParser::Check()
	{
		if (Advance(false))
		{
			unsigned int version;
			if (std::sscanf(&m_currentLine[0], " MD5Version %u", &version) == 1)
			{
				if (version == 10)
					return Ternary_True;
			}
		}

		return Ternary_False;
	}

	unsigned int MD5AnimParser::GetAnimatedComponentCount() const
	{
		return m_animatedComponents.size();
	}

	const MD5AnimParser::Frame* MD5AnimParser::GetFrames() const
	{
		return m_frames.data();
	}

	unsigned int MD5AnimParser::GetFrameCount() const
	{
		return m_frames.size();
	}

	unsigned int MD5AnimParser::GetFrameRate() const
	{
		return m_frameRate;
	}

	const MD5AnimParser::Joint* MD5AnimParser::GetJoints() const
	{
		return m_joints.data();
	}

	unsigned int MD5AnimParser::GetJointCount() const
	{
		return m_joints.size();
	}

	bool MD5AnimParser::Parse()
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
				#endif

				case 'b': // baseframe/bounds
					if (m_currentLine.StartsWith("baseframe {"))
					{
						if (!ParseBaseframe())
						{
							Error("Failed to parse baseframe");
							return false;
						}
					}
					else if (m_currentLine.StartsWith("bounds {"))
					{
						if (!ParseBounds())
						{
							Error("Failed to parse bounds");
							return false;
						}
					}
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					else
						UnrecognizedLine();
					#endif
					break;

				#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
				case 'c': // commandline
					if (m_currentLine.GetWord(0) != "commandline")
						UnrecognizedLine();
					break;
				#endif

				case 'f':
				{
					unsigned int index;
					if (std::sscanf(&m_currentLine[0], "frame %u {", &index) == 1)
					{
						if (m_frameIndex != index)
						{
							Error("Unexpected frame index (expected " + String::Number(m_frameIndex) + ", got " + String::Number(index) + ')');
							return false;
						}

						if (!ParseFrame())
						{
							Error("Failed to parse frame");
							return false;
						}

						m_frameIndex++;
					}
					else if (std::sscanf(&m_currentLine[0], "frameRate %u", &m_frameRate) != 1)
					{
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						UnrecognizedLine();
						#endif
					}
					break;
				}

				case 'h': // hierarchy
					if (m_currentLine.StartsWith("hierarchy {"))
					{
						if (!ParseHierarchy())
						{
							Error("Failed to parse hierarchy");
							return false;
						}
					}
					#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
					else
						UnrecognizedLine();
					#endif
					break;

				case 'n': // num[Frames/Joints]
				{
					unsigned int count;
					if (std::sscanf(&m_currentLine[0], "numAnimatedComponents %u", &count) == 1)
					{
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						if (!m_animatedComponents.empty())
							Warning("Animated components count is already defined");
						#endif

						m_animatedComponents.resize(count);
					}
					else if (std::sscanf(&m_currentLine[0], "numFrames %u", &count) == 1)
					{
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						if (!m_frames.empty())
							Warning("Frame count is already defined");
						#endif

						m_frames.resize(count);
					}
					else if (std::sscanf(&m_currentLine[0], "numJoints %u", &count) == 1)
					{
						#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
						if (!m_joints.empty())
							Warning("Joint count is already defined");
						#endif

						m_joints.resize(count);
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

		unsigned int frameCount = m_frames.size();
		if (frameCount == 0)
		{
			NazaraError("Frame count is invalid or missing");
			return false;
		}

		unsigned int jointCount = m_joints.size();
		if (jointCount == 0)
		{
			NazaraError("Joint count is invalid or missing");
			return false;
		}

		if (m_frameIndex != frameCount)
		{
			NazaraError("Missing frame infos: [" + String::Number(m_frameIndex) + ',' + String::Number(frameCount) + ']');
			return false;
		}

		if (m_frameRate == 0)
		{
			NazaraWarning("Framerate is either invalid or missing, assuming a default value of 24");
			m_frameRate = 24;
		}

		return true;
	}

	bool MD5AnimParser::Advance(bool required)
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

	void MD5AnimParser::Error(const String& message)
	{
		NazaraError(message + " at line #" + String::Number(m_lineCount));
	}

	bool MD5AnimParser::ParseBaseframe()
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

			// Space is important for the buffer of \n
			if (std::sscanf(&m_currentLine[0], " ( %f %f %f ) ( %f %f %f )", &m_joints[i].bindPos.x, &m_joints[i].bindPos.y, &m_joints[i].bindPos.z,
																			&m_joints[i].bindOrient.x, &m_joints[i].bindOrient.y, &m_joints[i].bindOrient.z) != 6)
			{
				UnrecognizedLine(true);
				return false;
			}
		}

		if (!Advance())
			return false;

		if (m_currentLine != '}')
		{
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			Warning("Bounds braces closing not found");
			#endif

			// On tente de survivre à l'erreur
			m_keepLastLine = true;
		}

		return true;
	}

	bool MD5AnimParser::ParseBounds()
	{
		unsigned int frameCount = m_frames.size();
		if (frameCount == 0)
		{
			Error("Frame count is invalid or missing");
			return false;
		}

		for (unsigned int i = 0; i < frameCount; ++i)
		{
			if (!Advance())
				return false;

			Vector3f min, max;
			// Space is important for the buffer of \n
			if (std::sscanf(&m_currentLine[0], " ( %f %f %f ) ( %f %f %f )", &min.x, &min.y, &min.z, &max.x, &max.y, &max.z) != 6)
			{
				UnrecognizedLine(true);
				return false;
			}

			m_frames[i].bounds.Set(min, max);
		}

		if (!Advance())
			return false;

		if (m_currentLine != '}')
		{
			#if NAZARA_UTILITY_STRICT_RESOURCE_PARSING
			Warning("Bounds braces closing not found");
			#endif

			// On tente de survivre à l'erreur
			m_keepLastLine = true;
		}

		return true;
	}

	bool MD5AnimParser::ParseFrame()
	{
		unsigned int animatedComponentsCount = m_animatedComponents.size();
		if (animatedComponentsCount == 0)
		{
			Error("Animated components count is missing or invalid");
			return false;
		}

		unsigned int jointCount = m_joints.size();
		if (jointCount == 0)
		{
			Error("Joint count is invalid or missing");
			return false;
		}

		String line;

		unsigned int count = 0;
		do
		{
			if (!Advance())
				return false;

			unsigned int index = 0;
			unsigned int size = m_currentLine.GetSize();
			do
			{
				float f;
				int read;
				if (std::sscanf(&m_currentLine[index], "%f%n", &f, &read) != 1)
				{
					UnrecognizedLine(true);
					return false;
				}

				index += read;

				m_animatedComponents[count] = f;

				count++;
			}
			while (index < size);
		}
		while (count < animatedComponentsCount);

		m_frames[m_frameIndex].joints.resize(jointCount);

		for (unsigned int i = 0; i < jointCount; ++i)
		{
			Quaternionf jointOrient = m_joints[i].bindOrient;
			Vector3f jointPos = m_joints[i].bindPos;
			unsigned int j = 0;

			if (m_joints[i].flags & 1) // Px
				jointPos.x = m_animatedComponents[m_joints[i].index + j++];

			if (m_joints[i].flags & 2) // Py
				jointPos.y = m_animatedComponents[m_joints[i].index + j++];

			if (m_joints[i].flags & 4) // Pz
				jointPos.z = m_animatedComponents[m_joints[i].index + j++];

			if (m_joints[i].flags & 8) // Qx
				jointOrient.x = m_animatedComponents[m_joints[i].index + j++];

			if (m_joints[i].flags & 16) // Qy
				jointOrient.y = m_animatedComponents[m_joints[i].index + j++];

			if (m_joints[i].flags & 32) // Qz
				jointOrient.z = m_animatedComponents[m_joints[i].index + j++];

			jointOrient.ComputeW();

			m_frames[m_frameIndex].joints[i].orient = jointOrient;
			m_frames[m_frameIndex].joints[i].pos = jointPos;
		}

		if (!Advance(false))
			return true;

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

	bool MD5AnimParser::ParseHierarchy()
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
			if (pos == String::npos)
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
			if (std::sscanf(&m_currentLine[0], "%63s %d %u %u", &name[0], &m_joints[i].parent, &m_joints[i].flags, &m_joints[i].index) != 4)
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
					Error("Joint's parent is out of bounds (" + String::Number(parent) + " >= " + String::Number(jointCount) + ')');
					return false;
				}
			}
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

	void MD5AnimParser::Warning(const String& message)
	{
		NazaraWarning(message + " at line #" + String::Number(m_lineCount));
	}

	void MD5AnimParser::UnrecognizedLine(bool error)
	{
		String message = "Unrecognized \"" + m_currentLine + '"';

		if (error)
			Error(message);
		else
			Warning(message);
		}
}

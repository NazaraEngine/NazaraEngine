// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Loaders/MD5Anim/Parser.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <cstdio>
#include <cstring>
#include <limits>
#include <Nazara/Utility/Debug.hpp>

NzMD5AnimParser::NzMD5AnimParser(NzInputStream& stream, const NzAnimationParams& parameters) :
m_stream(stream),
m_parameters(parameters),
m_keepLastLine(false),
m_frameIndex(0),
m_frameRate(0),
m_lineCount(0),
m_streamFlags(stream.GetStreamOptions())
{
	if ((m_streamFlags & nzStreamOption_Text) == 0)
		m_stream.SetStreamOptions(m_streamFlags | nzStreamOption_Text);
}

NzMD5AnimParser::~NzMD5AnimParser()
{
	if ((m_streamFlags & nzStreamOption_Text) == 0)
		m_stream.SetStreamOptions(m_streamFlags);
}

nzTernary NzMD5AnimParser::Check()
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

bool NzMD5AnimParser::Parse(NzAnimation* animation)
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
						Error("Unexpected frame index (expected " + NzString::Number(m_frameIndex) + ", got " + NzString::Number(index) + ')');
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
		NazaraError("Missing frame infos: [" + NzString::Number(m_frameIndex) + ',' + NzString::Number(frameCount) + ']');
		return false;
	}

	if (m_frameRate == 0)
	{
		NazaraWarning("Framerate is either invalid or missing, assuming a default value of 24");
		m_frameRate = 24;
	}

	// À ce stade, nous sommes censés avoir assez d'informations pour créer l'animation
	if (!animation->CreateSkeletal(frameCount, jointCount))
	{
		NazaraError("Failed to create animation");
		return false;
	}

	NzSequence sequence;
	sequence.firstFrame = 0;
	sequence.frameCount = m_frames.size();
	sequence.frameRate = m_frameRate;
	sequence.name = m_stream.GetPath().SubStringFrom(NAZARA_DIRECTORY_SEPARATOR, -1, true);
	if (!animation->AddSequence(sequence))
		NazaraWarning("Failed to add sequence");

	NzSequenceJoint* sequenceJoints = animation->GetSequenceJoints();

	// Pour que le squelette soit correctement aligné, il faut appliquer un quaternion "de correction" aux joints à la base du squelette
	NzQuaternionf rotationQuat = NzEulerAnglesf(-90.f, 90.f, 0.f);
	for (unsigned int i = 0; i < jointCount; ++i)
	{
		int parent = m_joints[i].parent;
		for (unsigned int j = 0; j < frameCount; ++j)
		{
			NzSequenceJoint& sequenceJoint = sequenceJoints[j*jointCount + i];

			if (parent >= 0)
			{
				sequenceJoint.position = m_frames[j].joints[i].pos;
				sequenceJoint.rotation = m_frames[j].joints[i].orient;
			}
			else
			{
				sequenceJoint.position = rotationQuat * m_frames[j].joints[i].pos;
				sequenceJoint.rotation = rotationQuat * m_frames[j].joints[i].orient;
			}

			sequenceJoint.scale.Set(1.f);
		}
	}

	return true;
}

bool NzMD5AnimParser::Advance(bool required)
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

void NzMD5AnimParser::Error(const NzString& message)
{
	NazaraError(message + " at line #" + NzString::Number(m_lineCount));
}

bool NzMD5AnimParser::ParseBaseframe()
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

		if (std::sscanf(&m_currentLine[0], "( %f %f %f ) ( %f %f %f )", &m_joints[i].bindPos.x, &m_joints[i].bindPos.y, &m_joints[i].bindPos.z,
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

bool NzMD5AnimParser::ParseBounds()
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

		NzVector3f min, max;
		if (std::sscanf(&m_currentLine[0], "( %f %f %f ) ( %f %f %f )", &min.x, &min.y, &min.z, &max.x, &max.y, &max.z) != 6)
		{
			UnrecognizedLine(true);
			return false;
		}

		m_frames[i].aabb.Set(min, max);
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

bool NzMD5AnimParser::ParseFrame()
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

	NzString line;

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
		NzQuaternionf jointOrient = m_joints[i].bindOrient;
		NzVector3f jointPos = m_joints[i].bindPos;
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

bool NzMD5AnimParser::ParseHierarchy()
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
		if (std::sscanf(&m_currentLine[0], "%s %d %u %u", &name[0], &m_joints[i].parent, &m_joints[i].flags, &m_joints[i].index) != 4)
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

void NzMD5AnimParser::Warning(const NzString& message)
{
	NazaraWarning(message + " at line #" + NzString::Number(m_lineCount));
}

void NzMD5AnimParser::UnrecognizedLine(bool error)
{
	NzString message = "Unrecognized \"" + m_currentLine + '"';

	if (error)
		Error(message);
	else
		Warning(message);
}

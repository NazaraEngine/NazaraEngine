// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Loaders/MD5Anim.hpp>
#include <Nazara/Utility/Loaders/MD5Anim/Parser.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	bool IsSupported(const NzString& extension)
	{
		return (extension == "md5anim");
	}

	nzTernary Check(NzInputStream& stream, const NzAnimationParams& parameters)
	{
		NzMD5AnimParser parser(stream);
		return parser.Check();
	}

	bool Load(NzAnimation* animation, NzInputStream& stream, const NzAnimationParams& parameters)
	{
		NzMD5AnimParser parser(stream);

		if (!parser.Parse())
		{
			NazaraError("MD5Anim parser failed");
			return false;
		}

		const NzMD5AnimParser::Frame* frames = parser.GetFrames();
		unsigned int frameCount = parser.GetFrameCount();
		unsigned int frameRate = parser.GetFrameRate();
		const NzMD5AnimParser::Joint* joints = parser.GetJoints();
		unsigned int jointCount = parser.GetJointCount();

		// À ce stade, nous sommes censés avoir assez d'informations pour créer l'animation
		animation->CreateSkeletal(frameCount, jointCount);

		NzSequence sequence;
		sequence.firstFrame = 0;
		sequence.frameCount = frameCount;
		sequence.frameRate = frameRate;
		sequence.name = stream.GetPath().SubStringFrom(NAZARA_DIRECTORY_SEPARATOR, -1, true);

		animation->AddSequence(sequence);

		NzSequenceJoint* sequenceJoints = animation->GetSequenceJoints();

		// Pour que le squelette soit correctement aligné, il faut appliquer un quaternion "de correction" aux joints à la base du squelette
		NzQuaternionf rotationQuat = NzEulerAnglesf(-90.f, 90.f, 0.f);
		for (unsigned int i = 0; i < jointCount; ++i)
		{
			int parent = joints[i].parent;
			for (unsigned int j = 0; j < frameCount; ++j)
			{
				NzSequenceJoint& sequenceJoint = sequenceJoints[j*jointCount + i];

				if (parent >= 0)
				{
					sequenceJoint.position = frames[j].joints[i].pos;
					sequenceJoint.rotation = frames[j].joints[i].orient;
				}
				else
				{
					sequenceJoint.position = rotationQuat * frames[j].joints[i].pos;
					sequenceJoint.rotation = rotationQuat * frames[j].joints[i].orient;
				}

				sequenceJoint.scale.Set(1.f);
			}
		}
	}
}

void NzLoaders_MD5Anim_Register()
{
	NzAnimationLoader::RegisterLoader(IsSupported, Check, Load);
}

void NzLoaders_MD5Anim_Unregister()
{
	NzAnimationLoader::UnregisterLoader(IsSupported, Check, Load);
}

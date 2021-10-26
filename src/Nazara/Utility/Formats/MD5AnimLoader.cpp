// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/MD5AnimLoader.hpp>
#include <Nazara/Utility/Formats/MD5AnimParser.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Utility/Sequence.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		bool IsSupported(const std::string_view& extension)
		{
			return (extension == "md5anim");
		}

		Ternary Check(Stream& stream, const AnimationParams& parameters)
		{
			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativeMD5AnimLoader", &skip) && skip)
				return Ternary::False;

			MD5AnimParser parser(stream);
			return parser.Check();
		}

		std::shared_ptr<Animation> Load(Stream& stream, const AnimationParams& /*parameters*/)
		{
			///TODO: Utiliser les paramètres
			MD5AnimParser parser(stream);

			if (!parser.Parse())
			{
				NazaraError("MD5Anim parser failed");
				return nullptr;
			}

			const MD5AnimParser::Frame* frames = parser.GetFrames();
			std::size_t frameCount = parser.GetFrameCount();
			std::size_t frameRate = parser.GetFrameRate();
			const MD5AnimParser::Joint* joints = parser.GetJoints();
			std::size_t jointCount = parser.GetJointCount();

			// À ce stade, nous sommes censés avoir assez d'informations pour créer l'animation
			std::shared_ptr<Animation> animation = std::make_shared<Animation>();
			animation->CreateSkeletal(frameCount, jointCount);

			Sequence sequence;
			sequence.firstFrame = 0;
			sequence.frameCount = frameCount;
			sequence.frameRate = frameRate;
			sequence.name = stream.GetPath().filename().generic_u8string();

			animation->AddSequence(sequence);

			SequenceJoint* sequenceJoints = animation->GetSequenceJoints();

			// Pour que le squelette soit correctement aligné, il faut appliquer un quaternion "de correction" aux joints à la base du squelette
			Quaternionf rotationQuat = Quaternionf::RotationBetween(Vector3f::UnitX(), Vector3f::Forward()) *
			                           Quaternionf::RotationBetween(Vector3f::UnitZ(), Vector3f::Up());

			for (std::size_t i = 0; i < jointCount; ++i)
			{
				int parent = joints[i].parent;
				for (std::size_t j = 0; j < frameCount; ++j)
				{
					SequenceJoint& sequenceJoint = sequenceJoints[j*jointCount + i];

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

			return animation;
		}
	}

	namespace Loaders
	{
		AnimationLoader::Entry GetAnimationLoader_MD5Anim()
		{
			AnimationLoader::Entry loader;
			loader.extensionSupport = IsSupported;
			loader.streamChecker = Check;
			loader.streamLoader = Load;

			return loader;
		}
	}
}

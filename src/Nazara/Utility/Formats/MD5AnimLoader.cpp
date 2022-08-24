// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/MD5AnimLoader.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Utility/Sequence.hpp>
#include <Nazara/Utility/Formats/MD5AnimParser.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		bool IsMD5AnimSupported(const std::string_view& extension)
		{
			return (extension == "md5anim");
		}

		Ternary CheckMD5Anim(Stream& stream, const AnimationParams& parameters)
		{
			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipBuiltinMD5AnimLoader", &skip) && skip)
				return Ternary::False;

			MD5AnimParser parser(stream);
			return parser.Check();
		}

		std::shared_ptr<Animation> LoadMD5Anim(Stream& stream, const AnimationParams& /*parameters*/)
		{
			///TODO: Utiliser les paramètres
			MD5AnimParser parser(stream);

			if (!parser.Parse())
			{
				NazaraError("MD5Anim parser failed");
				return nullptr;
			}

			const MD5AnimParser::Frame* frames = parser.GetFrames();
			UInt32 frameCount = parser.GetFrameCount();
			UInt32 frameRate = parser.GetFrameRate();
			const MD5AnimParser::Joint* joints = parser.GetJoints();
			UInt32 jointCount = parser.GetJointCount();

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

			//Matrix4f matrix = Matrix4f::Transform(Nz::Vector3f::Zero(), rotationQuat, Vector3f(1.f / 40.f));
			//matrix *= parameters.matrix;

			rotationQuat = Quaternionf::Identity();

			for (UInt32 frameIndex = 0; frameIndex < frameCount; ++frameIndex)
			{
				for (UInt32 jointIndex = 0; jointIndex < jointCount; ++jointIndex)
				{
					SequenceJoint& sequenceJoint = sequenceJoints[frameIndex * jointCount + jointIndex];

					Int32 parentId = joints[jointIndex].parent;
					if (parentId >= 0)
					{
						sequenceJoint.position = frames[frameIndex].joints[jointIndex].pos;
						sequenceJoint.rotation = frames[frameIndex].joints[jointIndex].orient;
					}
					else
					{
						sequenceJoint.position = rotationQuat * frames[frameIndex].joints[jointIndex].pos;
						sequenceJoint.rotation = rotationQuat * frames[frameIndex].joints[jointIndex].orient;
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
			loader.extensionSupport = IsMD5AnimSupported;
			loader.streamChecker = CheckMD5Anim;
			loader.streamLoader = LoadMD5Anim;

			return loader;
		}
	}
}

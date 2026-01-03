// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Formats/MD5AnimLoader.hpp>
#include <Nazara/Core/Animation.hpp>
#include <Nazara/Core/Formats/MD5AnimParser.hpp>

namespace Nz
{
	namespace
	{
		bool IsMD5AnimSupported(std::string_view extension)
		{
			return extension == ".md5anim";
		}

		Result<std::shared_ptr<Animation>, ResourceLoadingError> LoadMD5Anim(Stream& stream, const AnimationParams& /*parameters*/)
		{
			// TODO: Use parameters

			MD5AnimParser parser(stream);

			UInt64 streamPos = stream.GetCursorPos();

			if (!parser.Check())
				return Err(ResourceLoadingError::Unrecognized);

			stream.SetCursorPos(streamPos);

			if (!parser.Parse())
			{
				NazaraError("MD5Anim parser failed");
				return Err(ResourceLoadingError::DecodingError);
			}

			const MD5AnimParser::Frame* frames = parser.GetFrames();
			UInt32 frameCount = parser.GetFrameCount();
			UInt32 frameRate = parser.GetFrameRate();
			const MD5AnimParser::Joint* joints = parser.GetJoints();
			UInt32 jointCount = parser.GetJointCount();

			// À ce stade, nous sommes censés avoir assez d'informations pour créer l'animation
			std::shared_ptr<Animation> animation = std::make_shared<Animation>();
			animation->CreateSkeletal(frameCount, jointCount);

			Animation::Sequence sequence;
			sequence.firstFrame = 0;
			sequence.frameCount = frameCount;
			sequence.frameRate = frameRate;
			sequence.name = PathToString(stream.GetPath().filename());

			animation->AddSequence(sequence);

			Animation::SequenceJoint* sequenceJoints = animation->GetSequenceJoints();

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
					Animation::SequenceJoint& sequenceJoint = sequenceJoints[frameIndex * jointCount + jointIndex];

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

					sequenceJoint.scale = Vector3f::Unit();
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
			loader.streamLoader = LoadMD5Anim;
			loader.parameterFilter = [](const AnimationParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinMD5AnimLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loader;
		}
	}
}

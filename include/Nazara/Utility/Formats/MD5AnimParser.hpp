// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORMATS_MD5ANIMPARSER_HPP
#define NAZARA_FORMATS_MD5ANIMPARSER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_UTILITY_API MD5AnimParser
	{
		public:
			struct FrameJoint
			{
				Quaternionf orient;
				Vector3f pos;
			};

			struct Frame
			{
				std::vector<FrameJoint> joints;
				Boxf bounds;
			};

			struct Joint
			{
				Int32 parent;
				Quaternionf bindOrient;
				std::string name;
				Vector3f bindPos;
				UInt32 flags;
				UInt32 index;
			};

			MD5AnimParser(Stream& stream);
			~MD5AnimParser();

			Ternary Check();

			UInt32 GetAnimatedComponentCount() const;
			const Frame* GetFrames() const;
			UInt32 GetFrameCount() const;
			UInt32 GetFrameRate() const;
			const Joint* GetJoints() const;
			UInt32 GetJointCount() const;

			bool Parse();

		private:
			bool Advance(bool required = true);
			void Error(const std::string& message);
			bool ParseBaseframe();
			bool ParseBounds();
			bool ParseFrame();
			bool ParseHierarchy();
			void Warning(const std::string& message);
			void UnrecognizedLine(bool error = false);

			std::vector<float> m_animatedComponents;
			std::vector<Frame> m_frames;
			std::vector<Joint> m_joints;
			Stream& m_stream;
			StreamOptionFlags m_streamFlags;
			std::string m_currentLine;
			bool m_keepLastLine;
			unsigned int m_frameIndex;
			unsigned int m_frameRate;
			unsigned int m_lineCount;
	};
}

#endif // NAZARA_FORMATS_MD5ANIMPARSER_HPP

// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORMATS_MD5ANIMPARSER_HPP
#define NAZARA_FORMATS_MD5ANIMPARSER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Animation.hpp>
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
				Quaternionf bindOrient;
				String name;
				Vector3f bindPos;
				int parent;
				unsigned int flags;
				unsigned int index;
			};

			MD5AnimParser(InputStream& stream);
			~MD5AnimParser();

			Ternary Check();

			unsigned int GetAnimatedComponentCount() const;
			const Frame* GetFrames() const;
			unsigned int GetFrameCount() const;
			unsigned int GetFrameRate() const;
			const Joint* GetJoints() const;
			unsigned int GetJointCount() const;

			bool Parse();

		private:
			bool Advance(bool required = true);
			void Error(const String& message);
			bool ParseBaseframe();
			bool ParseBounds();
			bool ParseFrame();
			bool ParseHierarchy();
			void Warning(const String& message);
			void UnrecognizedLine(bool error = false);

			std::vector<float> m_animatedComponents;
			std::vector<Frame> m_frames;
			std::vector<Joint> m_joints;
			InputStream& m_stream;
			String m_currentLine;
			bool m_keepLastLine;
			unsigned int m_frameIndex;
			unsigned int m_frameRate;
			unsigned int m_lineCount;
			unsigned int m_streamFlags;
	};
}

#endif // NAZARA_FORMATS_MD5ANIMPARSER_HPP

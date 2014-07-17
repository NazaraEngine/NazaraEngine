// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOADERS_MD5ANIM_PARSER_HPP
#define NAZARA_LOADERS_MD5ANIM_PARSER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <vector>

class NzMD5AnimParser
{
	public:
		struct FrameJoint
		{
			NzQuaternionf orient;
			NzVector3f pos;
		};

		struct Frame
		{
			std::vector<FrameJoint> joints;
			NzBoxf bounds;
		};

		struct Joint
		{
			NzQuaternionf bindOrient;
			NzString name;
			NzVector3f bindPos;
			int parent;
			unsigned int flags;
			unsigned int index;
		};

		NzMD5AnimParser(NzInputStream& stream);
		~NzMD5AnimParser();

		nzTernary Check();

		unsigned int GetAnimatedComponentCount() const;
		const Frame* GetFrames() const;
		unsigned int GetFrameCount() const;
		unsigned int GetFrameRate() const;
		const Joint* GetJoints() const;
		unsigned int GetJointCount() const;

		bool Parse();

	private:
		bool Advance(bool required = true);
		void Error(const NzString& message);
		bool ParseBaseframe();
		bool ParseBounds();
		bool ParseFrame();
		bool ParseHierarchy();
		void Warning(const NzString& message);
		void UnrecognizedLine(bool error = false);

		std::vector<float> m_animatedComponents;
		std::vector<Frame> m_frames;
		std::vector<Joint> m_joints;
		NzInputStream& m_stream;
		NzString m_currentLine;
		bool m_keepLastLine;
		unsigned int m_frameIndex;
		unsigned int m_frameRate;
		unsigned int m_lineCount;
		unsigned int m_streamFlags;
};

#endif // NAZARA_LOADERS_MD5ANIM_PARSER_HPP

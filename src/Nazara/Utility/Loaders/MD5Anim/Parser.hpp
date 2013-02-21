// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOADERS_MD5ANIM_PARSER_HPP
#define NAZARA_LOADERS_MD5ANIM_PARSER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Math/Cube.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <vector>

class NzMD5AnimParser
{
	public:
		NzMD5AnimParser(NzInputStream& stream, const NzAnimationParams& parameters);
		~NzMD5AnimParser();

		bool Check();
		bool Parse(NzAnimation* animation);

	private:
		struct Frame
		{
			struct Joint
			{
				NzQuaternionf orient;
				NzVector3f pos;
			};

			std::vector<Joint> joints;
			NzCubef aabb;
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
		const NzAnimationParams& m_parameters;
		bool m_keepLastLine;
		unsigned int m_frameIndex;
		unsigned int m_frameRate;
		unsigned int m_lineCount;
		unsigned int m_streamFlags;
};

#endif // NAZARA_LOADERS_MD5ANIM_PARSER_HPP

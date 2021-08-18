// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORMATS_MD5MESHPARSER_HPP
#define NAZARA_FORMATS_MD5MESHPARSER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Config.hpp>
#include <string>
#include <vector>

namespace Nz
{
	class NAZARA_UTILITY_API MD5MeshParser
	{
		public:
			struct Joint
			{
				std::string name;
				Int32 parent;
				Quaternionf bindOrient;
				Vector3f bindPos;
			};

			using Triangle = Vector3ui;

			struct Vertex
			{
				Vector2f uv;
				unsigned int startWeight;
				unsigned int weightCount;
			};

			struct Weight
			{
				Vector3f pos;
				float bias;
				unsigned int joint;
			};

			struct Mesh
			{
				std::vector<Triangle> triangles;
				std::vector<Vertex> vertices;
				std::vector<Weight> weights;
				std::string shader;
			};

			MD5MeshParser(Stream& stream);
			~MD5MeshParser();

			Ternary Check();

			const Joint* GetJoints() const;
			UInt32 GetJointCount() const;
			const Mesh* GetMeshes() const;
			UInt32 GetMeshCount() const;

			bool Parse();

		private:
			bool Advance(bool required = true);
			void Error(const std::string& message);
			bool ParseJoints();
			bool ParseMesh();
			void Warning(const std::string& message);
			void UnrecognizedLine(bool error = false);

			std::vector<Joint> m_joints;
			std::vector<Mesh> m_meshes;
			Stream& m_stream;
			StreamOptionFlags m_streamFlags;
			std::string m_currentLine;
			bool m_keepLastLine;
			unsigned int m_lineCount;
			unsigned int m_meshIndex;
	};
}

#endif // NAZARA_FORMATS_MD5MESHPARSER_HPP

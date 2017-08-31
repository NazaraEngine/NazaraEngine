// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORMATS_MD5MESHPARSER_HPP
#define NAZARA_FORMATS_MD5MESHPARSER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_UTILITY_API MD5MeshParser
	{
		public:
			struct Joint
			{
				Int32 parent;
				Quaternionf bindOrient;
				String name;
				Vector3f bindPos;
			};

			typedef Vector3ui Triangle;

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
				String shader;
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
			void Error(const String& message);
			bool ParseJoints();
			bool ParseMesh();
			void Warning(const String& message);
			void UnrecognizedLine(bool error = false);

			std::vector<Joint> m_joints;
			std::vector<Mesh> m_meshes;
			Stream& m_stream;
			String m_currentLine;
			bool m_keepLastLine;
			unsigned int m_lineCount;
			unsigned int m_meshIndex;
			unsigned int m_streamFlags;
	};
}

#endif // NAZARA_FORMATS_MD5MESHPARSER_HPP

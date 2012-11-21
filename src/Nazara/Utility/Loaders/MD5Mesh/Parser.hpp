// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOADERS_MD5MESH_PARSER_HPP
#define NAZARA_LOADERS_MD5MESH_PARSER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <vector>

class NzMD5MeshParser
{
	public:
		NzMD5MeshParser(NzInputStream& stream, const NzMeshParams& parameters);
		~NzMD5MeshParser();

		bool Check();
		bool Parse(NzMesh* mesh);

	private:
		struct Joint
		{
			NzQuaternionf bindOrient;
			NzString name;
			NzVector3f bindPos;
			int parent;
		};

		struct Mesh
		{
			typedef NzVector3ui Triangle;

			struct Vertex
			{
				NzVector2f uv;
				unsigned int startWeight;
				unsigned int weightCount;
			};

			struct Weight
			{
				NzVector3f pos;
				float bias;
				unsigned int joint;
			};

			std::vector<Triangle> triangles;
			std::vector<Vertex> vertices;
			std::vector<Weight> weights;
			NzString shader;
		};

		bool Advance(bool required = true);
		void Error(const NzString& message);
		bool ParseJoints();
		bool ParseMesh();
		void Warning(const NzString& message);
		void UnrecognizedLine(bool error = false);

		std::vector<Joint> m_joints;
		std::vector<Mesh> m_meshes;
		NzInputStream& m_stream;
		NzString m_currentLine;
		const NzMeshParams& m_parameters;
		bool m_keepLastLine;
		unsigned int m_lineCount;
		unsigned int m_meshIndex;
		unsigned int m_streamFlags;
};

#endif // NAZARA_LOADERS_MD5MESH_PARSER_HPP

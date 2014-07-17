// Copyright (C) 2014 Jérôme Leclercq
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
		struct Joint
		{
			NzQuaternionf bindOrient;
			NzString name;
			NzVector3f bindPos;
			int parent;
		};

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

		struct Mesh
		{
			std::vector<Triangle> triangles;
			std::vector<Vertex> vertices;
			std::vector<Weight> weights;
			NzString shader;
		};

		NzMD5MeshParser(NzInputStream& stream);
		~NzMD5MeshParser();

		nzTernary Check();

		const Joint* GetJoints() const;
		unsigned int GetJointCount() const;
		const Mesh* GetMeshes() const;
		unsigned int GetMeshCount() const;

		bool Parse();

	private:
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
		bool m_keepLastLine;
		unsigned int m_lineCount;
		unsigned int m_meshIndex;
		unsigned int m_streamFlags;
};

#endif // NAZARA_LOADERS_MD5MESH_PARSER_HPP

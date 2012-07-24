// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOADERS_MD2_MESH_HPP
#define NAZARA_LOADERS_MD2_MESH_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/KeyframeMesh.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Utility/Loaders/MD2/Constants.hpp>

class NzIndexBuffer;
class NzInputStream;
class NzVertexBuffer;
struct NzMeshParams;

class NAZARA_API NzMD2Mesh : public NzKeyframeMesh
{
	public:
		NzMD2Mesh(const NzMesh* parent);
		~NzMD2Mesh();

		bool Create(const md2_header& header, NzInputStream& stream, const NzMeshParams& parameters);
		void Destroy();

		nzAnimationType GetAnimationType() const;
		unsigned int GetFrameCount() const;
		const NzIndexBuffer* GetIndexBuffer() const;
		nzPrimitiveType GetPrimitiveType() const;
		const NzVertexBuffer* GetVertexBuffer() const;
		const NzVertexDeclaration* GetVertexDeclaration() const;

		bool IsAnimated() const;

		static void Initialize();
		static void Uninitialize();

	private:
		void AnimateImpl(unsigned int frameA, unsigned int frameB, float interpolation);

		struct Frame
		{
			//AxisAlignedBox aabb;
			nzUInt8* normal;
			NzVector3f* tangents;
			NzVector3f* vertices;
			char name[16];
		};

		//AxisAlignedBox m_aabb;
		Frame* m_frames;
		NzIndexBuffer* m_indexBuffer;
		NzVertexBuffer* m_vertexBuffer;
		unsigned int m_frameCount;
		unsigned int m_vertexCount;

		static NzVertexDeclaration s_declaration;
};

#endif // NAZARA_LOADERS_MD2_MESH_HPP

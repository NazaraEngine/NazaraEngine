// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEPTHRENDERQUEUE_HPP
#define NAZARA_DEPTHRENDERQUEUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/ForwardRenderQueue.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <map>
#include <tuple>

class NAZARA_GRAPHICS_API NzDepthRenderQueue : public NzAbstractRenderQueue
{
	public:
		NzDepthRenderQueue() = default;
		~NzDepthRenderQueue() = default;

		void AddBillboard(const NzMaterial* material, const NzVector3f& position, const NzVector2f& size, const NzVector2f& sinCos = NzVector2f(0.f, 1.f), const NzColor& color = NzColor::White) override;
		void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr = nullptr, NzSparsePtr<const NzColor> colorPtr = nullptr) override;
		void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const float> alphaPtr) override;
		void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const NzColor> colorPtr = nullptr) override;
		void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const float> alphaPtr) override;
		void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr = nullptr, NzSparsePtr<const NzColor> colorPtr = nullptr) override;
		void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const float> alphaPtr) override;
		void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const NzColor> colorPtr = nullptr) override;
		void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const float> alphaPtr) override;
		void AddDrawable(const NzDrawable* drawable) override;
		void AddMesh(const NzMaterial* material, const NzMeshData& meshData, const NzBoxf& meshAABB, const NzMatrix4f& transformMatrix) override;
		void AddSprites(const NzMaterial* material, const NzVertexStruct_XYZ_Color_UV* vertices, unsigned int spriteCount, const NzTexture* overlay = nullptr) override;

		void Clear(bool fully = false);

		struct BillboardData
		{
			NzColor color;
			NzVector3f center;
			NzVector2f size;
			NzVector2f sinCos;
		};

		struct MeshInstanceEntry
		{
			NazaraSlot(NzIndexBuffer, OnIndexBufferRelease, indexBufferReleaseSlot);
			NazaraSlot(NzVertexBuffer, OnVertexBufferRelease, vertexBufferReleaseSlot);

			std::vector<NzMatrix4f> instances;
		};

		struct SpriteChain_XYZ_Color_UV
		{
			const NzVertexStruct_XYZ_Color_UV* vertices;
			unsigned int spriteCount;
		};

		std::map<NzMeshData, MeshInstanceEntry, NzForwardRenderQueue::MeshDataComparator> meshes;
		std::vector<BillboardData> billboards;
		std::vector<const NzDrawable*> otherDrawables;
		std::vector<SpriteChain_XYZ_Color_UV> basicSprites;

	private:
		bool IsMaterialSuitable(const NzMaterial* material) const;

		void OnIndexBufferInvalidation(const NzIndexBuffer* indexBuffer);
		void OnVertexBufferInvalidation(const NzVertexBuffer* vertexBuffer);
};

#endif // NAZARA_DEPTHRENDERQUEUE_HPP

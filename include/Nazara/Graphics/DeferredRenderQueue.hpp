// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDRENDERQUEUE_HPP
#define NAZARA_DEFERREDRENDERQUEUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ObjectListener.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/MeshData.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <map>
#include <tuple>

class NzForwardRenderQueue;

class NAZARA_API NzDeferredRenderQueue : public NzAbstractRenderQueue
{
	public:
		NzDeferredRenderQueue(NzForwardRenderQueue* forwardQueue);
		~NzDeferredRenderQueue() = default;

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

		struct MeshDataComparator
		{
			bool operator()(const NzMeshData& data1, const NzMeshData& data2);
		};

		struct MeshInstanceEntry
		{
			NazaraSlot(NzIndexBuffer, OnIndexBufferRelease, indexBufferReleaseSlot);
			NazaraSlot(NzVertexBuffer, OnVertexBufferRelease, vertexBufferReleaseSlot);

			std::vector<NzMatrix4f> instances;
		};

		typedef std::map<NzMeshData, MeshInstanceEntry, MeshDataComparator> MeshInstanceContainer;

		struct BatchedModelMaterialComparator
		{
			bool operator()(const NzMaterial* mat1, const NzMaterial* mat2);
		};

		struct BatchedModelEntry
		{
			NazaraSlot(NzMaterial, OnMaterialRelease, materialReleaseSlot);

			MeshInstanceContainer meshMap;
			bool enabled = false;
			bool instancingEnabled = false;
		};

		typedef std::map<const NzMaterial*, BatchedModelEntry, BatchedModelMaterialComparator> ModelBatches;

		ModelBatches opaqueModels;
		NzForwardRenderQueue* m_forwardQueue;

		void OnIndexBufferInvalidation(const NzIndexBuffer* indexBuffer);
		void OnMaterialInvalidation(const NzMaterial* material);
		void OnVertexBufferInvalidation(const NzVertexBuffer* vertexBuffer);
};

#endif // NAZARA_DEFERREDRENDERQUEUE_HPP

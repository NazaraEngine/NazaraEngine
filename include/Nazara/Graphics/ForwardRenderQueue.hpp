// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORWARDRENDERQUEUE_HPP
#define NAZARA_FORWARDRENDERQUEUE_HPP

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

class NzAbstractViewer;

class NAZARA_API NzForwardRenderQueue : public NzAbstractRenderQueue, NzObjectListener
{
	friend class NzForwardRenderTechnique;

	public:
		NzForwardRenderQueue() = default;
		~NzForwardRenderQueue() = default;

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
		void AddLight(const NzLight* light) override;
		void AddMesh(const NzMaterial* material, const NzMeshData& meshData, const NzBoxf& meshAABB, const NzMatrix4f& transformMatrix) override;
		void AddSprites(const NzMaterial* material, const NzVertexStruct_XYZ_Color_UV* vertices, unsigned int spriteCount, const NzTexture* overlay = nullptr) override;

		void Clear(bool fully);

		void Sort(const NzAbstractViewer* viewer);

	private:
		bool OnObjectDestroy(const NzRefCounted* object, int index) override;
		void OnObjectReleased(const NzRefCounted* object, int index) override;

		/// Billboards
		struct BillboardData
		{
			NzColor color;
			NzVector3f center;
			NzVector2f size;
			NzVector2f sinCos;
		};

		struct BatchedBillboardComparator
		{
			bool operator()(const NzMaterial* mat1, const NzMaterial* mat2);
		};

		struct BatchedBillboardEntry
		{
			BatchedBillboardEntry(NzObjectListener* listener, int materialValue) :
			materialListener(listener, materialValue)
			{
			}

			NzMaterialConstListener materialListener;
			std::vector<BillboardData> billboards;
		};

		typedef std::map<const NzMaterial*, BatchedBillboardEntry, BatchedBillboardComparator> BatchedBillboardContainer;

		/// Sprites
		struct SpriteChain_XYZ_Color_UV
		{
			const NzVertexStruct_XYZ_Color_UV* vertices;
			unsigned int spriteCount;
		};

		struct BatchedSpriteEntry
		{
			BatchedSpriteEntry(NzObjectListener* listener, int textureValue) :
			textureListener(listener, textureValue)
			{
			}

			std::vector<SpriteChain_XYZ_Color_UV> spriteChains;
			NzTextureConstListener textureListener;
		};

		struct BatchedSpriteMaterialComparator
		{
			bool operator()(const NzMaterial* mat1, const NzMaterial* mat2);
		};

		typedef std::map<const NzTexture*, BatchedSpriteEntry> BasicSpriteOverlayContainer;

		struct BatchedBasicSpriteEntry
		{
			BatchedBasicSpriteEntry(NzObjectListener* listener, int materialValue) :
			materialListener(listener, materialValue)
			{
			}

			NzMaterialConstListener materialListener;
			BasicSpriteOverlayContainer overlayMap;
			bool enabled = false;
		};

		typedef std::map<const NzMaterial*, BatchedBasicSpriteEntry> BasicSpriteBatches;

		/// Meshes
		struct MeshDataComparator
		{
			bool operator()(const NzMeshData& data1, const NzMeshData& data2);
		};

		struct MeshInstanceEntry
		{
			MeshInstanceEntry(NzObjectListener* listener, int indexBufferValue, int vertexBufferValue) :
			indexBufferListener(listener, indexBufferValue),
			vertexBufferListener(listener, vertexBufferValue)
			{
			}

			std::vector<NzMatrix4f> instances;
			NzIndexBufferConstListener indexBufferListener;
			NzSpheref squaredBoundingSphere;
			NzVertexBufferConstListener vertexBufferListener;
		};

		typedef std::map<NzMeshData, MeshInstanceEntry, MeshDataComparator> MeshInstanceContainer;

		struct BatchedModelMaterialComparator
		{
			bool operator()(const NzMaterial* mat1, const NzMaterial* mat2);
		};

		struct BatchedModelEntry
		{
			BatchedModelEntry(NzObjectListener* listener, int materialValue) :
			materialListener(listener, materialValue)
			{
			}

			NzMaterialConstListener materialListener;
			MeshInstanceContainer meshMap;
			bool enabled = false;
			bool instancingEnabled = false;
		};

		typedef std::map<const NzMaterial*, BatchedModelEntry, BatchedModelMaterialComparator> ModelBatches;

		struct TransparentModelData
		{
			NzMatrix4f transformMatrix;
			NzMeshData meshData;
			NzSpheref squaredBoundingSphere;
			const NzMaterial* material;
		};

		typedef std::vector<const NzLight*> LightContainer;
		typedef std::vector<unsigned int> TransparentModelContainer;

		BatchedBillboardContainer billboards;
		BasicSpriteBatches basicSprites;
		ModelBatches opaqueModels;
		TransparentModelContainer transparentModels;
		std::vector<TransparentModelData> transparentModelData;
		std::vector<const NzDrawable*> otherDrawables;
		LightContainer directionalLights;
		LightContainer lights;
};

#endif // NAZARA_FORWARDRENDERQUEUE_HPP

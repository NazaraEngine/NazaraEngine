// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORWARDRENDERQUEUE_HPP
#define NAZARA_FORWARDRENDERQUEUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ResourceListener.hpp>
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

class NAZARA_API NzForwardRenderQueue : public NzAbstractRenderQueue, NzResourceListener
{
	friend class NzForwardRenderTechnique;

	public:
		NzForwardRenderQueue() = default;
		~NzForwardRenderQueue() = default;

		void AddDrawable(const NzDrawable* drawable) override;
		void AddLight(const NzLight* light) override;
		void AddMesh(const NzMaterial* material, const NzMeshData& meshData, const NzBoxf& meshAABB, const NzMatrix4f& transformMatrix) override;
		void AddSprites(const NzMaterial* material, const NzVertexStruct_XYZ_Color_UV* vertices, unsigned int spriteCount, const NzTexture* overlay = nullptr) override;

		void Clear(bool fully);

		void Sort(const NzAbstractViewer* viewer);

	private:
		bool OnResourceDestroy(const NzResource* resource, int index) override;
		void OnResourceReleased(const NzResource* resource, int index) override;

		struct SpriteChain_XYZ_Color_UV
		{
			const NzVertexStruct_XYZ_Color_UV* vertices;
			unsigned int spriteCount;
		};

		struct BatchedSpriteEntry
		{
			BatchedSpriteEntry(NzForwardRenderQueue* listener, int textureValue) :
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
			BatchedBasicSpriteEntry(NzForwardRenderQueue* listener, int materialValue) :
			materialListener(listener, materialValue)
			{
			}

			NzMaterialConstListener materialListener;
			BasicSpriteOverlayContainer overlayMap;
			bool enabled = false;
		};

		typedef std::map<const NzMaterial*, BatchedBasicSpriteEntry> BasicSpriteBatches;

		struct MeshDataComparator
		{
			bool operator()(const NzMeshData& data1, const NzMeshData& data2);
		};

		struct MeshInstanceEntry
		{
			MeshInstanceEntry(NzForwardRenderQueue* listener, int indexBufferValue, int vertexBufferValue) :
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
			BatchedModelEntry(NzForwardRenderQueue* listener, int materialValue) :
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

		BasicSpriteBatches basicSprites;
		ModelBatches opaqueModels;
		TransparentModelContainer transparentModels;
		std::vector<TransparentModelData> transparentModelData;
		std::vector<const NzDrawable*> otherDrawables;
		LightContainer directionalLights;
		LightContainer lights;
};

#endif // NAZARA_FORWARDRENDERQUEUE_HPP

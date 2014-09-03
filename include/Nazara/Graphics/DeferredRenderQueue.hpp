// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEFERREDRENDERQUEUE_HPP
#define NAZARA_DEFERREDRENDERQUEUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ResourceListener.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Utility/MeshData.hpp>
#include <map>
#include <tuple>

class NzForwardRenderQueue;
class NzMaterial;
class NzSkeletalMesh;
class NzStaticMesh;

class NAZARA_API NzDeferredRenderQueue : public NzAbstractRenderQueue, NzResourceListener
{
	public:
		NzDeferredRenderQueue(NzForwardRenderQueue* forwardQueue);
		~NzDeferredRenderQueue();

		void AddBillboard(const NzMaterial* material, const NzVector3f& position, const NzVector2f& size, const NzVector2f& sinCos = NzVector2f(0.f, 1.f), const NzColor& color = NzColor::White) override;
		void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr = nullptr, NzSparsePtr<const NzColor> colorPtr = nullptr) override;
		void AddDrawable(const NzDrawable* drawable) override;
		void AddLight(const NzLight* light) override;
		void AddMesh(const NzMaterial* material, const NzMeshData& meshData, const NzBoxf& meshAABB, const NzMatrix4f& transformMatrix) override;
		void AddSprite(const NzSprite* sprite) override;

		void Clear(bool fully);

		struct BatchedModelMaterialComparator
		{
			bool operator()(const NzMaterial* mat1, const NzMaterial* mat2);
		};

		struct BatchedSpriteMaterialComparator
		{
			bool operator()(const NzMaterial* mat1, const NzMaterial* mat2);
		};

		struct MeshDataComparator
		{
			bool operator()(const NzMeshData& data1, const NzMeshData& data2);
		};

		typedef std::map<NzMeshData, std::vector<NzMatrix4f>, MeshDataComparator> MeshInstanceContainer;
		typedef std::map<const NzMaterial*, std::tuple<bool, bool, MeshInstanceContainer>, BatchedModelMaterialComparator> ModelBatches;
		typedef std::map<const NzMaterial*, std::vector<const NzSprite*>> BatchedSpriteContainer;
		typedef std::vector<const NzLight*> LightContainer;

		ModelBatches opaqueModels;
		BatchedSpriteContainer sprites;
		LightContainer directionalLights;
		LightContainer pointLights;
		LightContainer spotLights;
		NzForwardRenderQueue* m_forwardQueue;

	private:
		bool OnResourceDestroy(const NzResource* resource, int index) override;
		void OnResourceReleased(const NzResource* resource, int index) override;
};

#endif // NAZARA_DEFERREDRENDERQUEUE_HPP

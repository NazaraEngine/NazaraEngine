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

		void AddDrawable(const NzDrawable* drawable) override;
		void AddLight(const NzLight* light) override;
		void AddSprite(const NzSprite* sprite) override;
		void AddSubMesh(const NzMaterial* material, const NzSubMesh* subMesh, const NzMatrix4f& transformMatrix) override;

		void Clear(bool fully);

		struct SkeletalData
		{
			///TODO
			NzMatrix4f transformMatrix;
		};

		struct StaticData
		{
			NzMatrix4f transformMatrix;
		};

		struct BatchedModelMaterialComparator
		{
			bool operator()(const NzMaterial* mat1, const NzMaterial* mat2);
		};

		struct BatchedSpriteMaterialComparator
		{
			bool operator()(const NzMaterial* mat1, const NzMaterial* mat2);
		};

		struct BatchedSkeletalMeshComparator
		{
			bool operator()(const NzSkeletalMesh* subMesh1, const NzSkeletalMesh* subMesh2);
		};

		struct BatchedStaticMeshComparator
		{
			bool operator()(const NzStaticMesh* subMesh1, const NzStaticMesh* subMesh2);
		};

		typedef std::map<const NzSkeletalMesh*, std::vector<SkeletalData>, BatchedSkeletalMeshComparator> BatchedSkeletalMeshContainer;
		typedef std::map<const NzStaticMesh*, std::vector<StaticData>, BatchedStaticMeshComparator> BatchedStaticMeshContainer;
		typedef std::map<const NzMaterial*, std::tuple<bool, bool, BatchedSkeletalMeshContainer, BatchedStaticMeshContainer>, BatchedModelMaterialComparator> BatchedModelContainer;
		typedef std::map<const NzMaterial*, std::vector<const NzSprite*>> BatchedSpriteContainer;
		typedef std::vector<const NzLight*> LightContainer;

		BatchedModelContainer opaqueModels;
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

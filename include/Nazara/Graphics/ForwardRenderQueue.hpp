// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORWARDRENDERQUEUE_HPP
#define NAZARA_FORWARDRENDERQUEUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ObjectListener.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Utility/MeshData.hpp>
#include <map>
#include <tuple>

class NzAbstractViewer;
class NzMaterial;
class NzSkeletalMesh;
class NzStaticMesh;

class NAZARA_API NzForwardRenderQueue : public NzAbstractRenderQueue, NzObjectListener
{
	friend class NzForwardRenderTechnique;

	public:
		NzForwardRenderQueue() = default;
		~NzForwardRenderQueue();

		void AddDrawable(const NzDrawable* drawable) override;
		void AddLight(const NzLight* light) override;
		void AddMesh(const NzMaterial* material, const NzMeshData& meshData, const NzBoxf& meshAABB, const NzMatrix4f& transformMatrix) override;
		void AddSprite(const NzSprite* sprite) override;

		void Clear(bool fully);

		void Sort(const NzAbstractViewer* viewer);

	private:
		bool OnObjectDestroy(const NzRefCounted* object, int index) override;
		void OnObjectReleased(const NzRefCounted* object, int index) override;

		struct TransparentModelData
		{
			NzMatrix4f transformMatrix;
			NzMeshData meshData;
			NzSpheref boundingSphere;
			const NzMaterial* material;
		};

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

		typedef std::map<NzMeshData, std::pair<NzSpheref, std::vector<NzMatrix4f>>, MeshDataComparator> MeshInstanceContainer;
		typedef std::map<const NzMaterial*, std::tuple<bool, bool, MeshInstanceContainer>, BatchedModelMaterialComparator> ModelBatches;
		typedef std::map<const NzMaterial*, std::vector<const NzSprite*>> BatchedSpriteContainer;
		typedef std::vector<const NzLight*> LightContainer;
		typedef std::vector<unsigned int> TransparentModelContainer;

		ModelBatches opaqueModels;
		BatchedSpriteContainer sprites;
		TransparentModelContainer transparentModels;
		std::vector<TransparentModelData> transparentModelData;
		std::vector<const NzDrawable*> otherDrawables;
		LightContainer directionalLights;
		LightContainer lights;
};

#endif // NAZARA_FORWARDRENDERQUEUE_HPP

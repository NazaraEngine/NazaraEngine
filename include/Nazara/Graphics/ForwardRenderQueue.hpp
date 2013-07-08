// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORWARDRENDERQUEUE_HPP
#define NAZARA_FORWARDRENDERQUEUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ResourceListener.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <map>

class NzCamera;
class NzMaterial;
class NzSkeletalMesh;
class NzStaticMesh;

class NAZARA_API NzForwardRenderQueue : public NzAbstractRenderQueue, NzResourceListener
{
	friend class NzForwardRenderTechnique;

	public:
		NzForwardRenderQueue() = default;
		~NzForwardRenderQueue();

		void AddDrawable(const NzDrawable* drawable);
		void AddLight(const NzLight* light);
		void AddModel(const NzModel* model);

		void Clear(bool fully);

		void Sort(const NzCamera& camera);

	private:
		void OnResourceDestroy(const NzResource* resource, int index);

		struct MaterialComparator
		{
			bool operator()(const NzMaterial* mat1, const NzMaterial* mat2);
		};

		struct SkeletalData
		{
			///TODO
			NzMatrix4f transformMatrix;
		};

		struct SkeletalMeshComparator
		{
			bool operator()(const NzSkeletalMesh* subMesh1, const NzSkeletalMesh* subMesh2);
		};

		struct StaticMeshComparator
		{
			bool operator()(const NzStaticMesh* subMesh1, const NzStaticMesh* subMesh2);
		};

		struct TransparentModel
		{
			NzBoxf aabb;
			NzMatrix4f transformMatrix;
			NzMaterial* material;
		};

		struct TransparentSkeletalModel : public TransparentModel
		{
			///TODO
		};

		struct TransparentStaticModel : public TransparentModel
		{
			NzStaticMesh* mesh;
		};

		typedef std::map<const NzSkeletalMesh*, std::vector<SkeletalData>, SkeletalMeshComparator> SkeletalMeshContainer;
		typedef std::map<const NzStaticMesh*, std::vector<NzMatrix4f>, StaticMeshComparator> StaticMeshContainer;
		typedef std::map<const NzMaterial*, std::pair<SkeletalMeshContainer, StaticMeshContainer>, MaterialComparator> MeshContainer;

		std::map<const NzMaterial*, std::vector<BillboardData>, MaterialComparator> billboards;
		MeshContainer opaqueModels;
		std::vector<std::pair<unsigned int, bool>> transparentsModels;
		std::vector<TransparentSkeletalModel> transparentSkeletalModels;
		std::vector<TransparentStaticModel> transparentStaticModels;
		std::vector<const NzDrawable*> otherDrawables;
		std::vector<const NzLight*> directionnalLights;
		std::vector<const NzLight*> lights;
};

#endif // NAZARA_FORWARDRENDERQUEUE_HPP

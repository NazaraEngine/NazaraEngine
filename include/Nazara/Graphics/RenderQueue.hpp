// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_RENDERQUEUE_HPP
#define NAZARA_RENDERQUEUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <map>
#include <vector>

class NzDrawable;
class NzLight;
class NzMaterial;
class NzModel;
class NzSkeletalMesh;
class NzStaticMesh;

class NAZARA_API NzRenderQueue
{
	public:
		struct MaterialComparator
		{
			bool operator()(const NzMaterial* mat1, const NzMaterial* mat2);
		};

		struct SkeletalMeshComparator
		{
			bool operator()(const NzSkeletalMesh* subMesh1, const NzSkeletalMesh* subMesh2);
		};

		struct StaticMeshComparator
		{
			bool operator()(const NzStaticMesh* subMesh1, const NzStaticMesh* subMesh2);
		};

		NzRenderQueue() = default;
		~NzRenderQueue() = default;

		void Clear();

		struct SkeletalData
		{
			NzMatrix4f transformMatrix;

			///TODO: Déplacer vers un container séparé qui ne serait pas sujer à Clear();
			std::vector<NzMeshVertex> skinnedVertices;
		};

		typedef std::map<NzSkeletalMesh*, std::vector<SkeletalData>, SkeletalMeshComparator> SkeletalMeshContainer;
		typedef std::map<NzStaticMesh*, std::vector<NzMatrix4f>, StaticMeshComparator> StaticMeshContainer;

		std::map<NzMaterial*, SkeletalMeshContainer, MaterialComparator> visibleSkeletalModels;
		std::map<NzMaterial*, StaticMeshContainer, MaterialComparator> visibleStaticModels;
		std::vector<const NzDrawable*> otherDrawables;
		std::vector<const NzLight*> directionnalLights;
		std::vector<const NzLight*> visibleLights;
};

#endif // NAZARA_RENDERQUEUE_HPP

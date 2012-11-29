// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MODEL_HPP
#define NAZARA_MODEL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/3D/SceneNode.hpp>
#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Utility/Mesh.hpp>

struct NzModelParameters
{
	bool loadAnimation = true;
	bool loadMaterials = true;
	NzAnimationParams animationParams;
	NzMaterialParams materialParams;
};

class NAZARA_API NzModel : public NzSceneNode
{
	public:
		NzModel();
		NzModel(const NzModel& model);
		~NzModel();

		const NzAnimation* GetAnimation() const;
		const NzMaterial* GetMaterial(unsigned int matIndex) const;
		const NzMaterial* GetMaterial(unsigned int skinIndex, unsigned int matIndex) const;
		unsigned int GetMaterialCount() const;
		unsigned int GetSkinCount() const;
		const NzMesh* GetMesh() const;
		nzSceneNodeType GetSceneNodeType() const override;
		const NzSkeleton* GetSkeleton() const;

		bool HasAnimation() const;

		bool LoadFromFile(const NzString& meshPath, const NzMeshParams& meshParameters = NzMeshParams(), const NzModelParameters& modelParameters = NzModelParameters());
		bool LoadFromMemory(const void* data, std::size_t size, const NzMeshParams& meshParameters = NzMeshParams(), const NzModelParameters& modelParameters = NzModelParameters());
		bool LoadFromStream(NzInputStream& stream, const NzMeshParams& meshParameters = NzMeshParams(), const NzModelParameters& modelParameters = NzModelParameters());

		void Reset();

		bool SetAnimation(const NzAnimation* animation);
		void SetMaterial(unsigned int matIndex, const NzMaterial* material);
		void SetMaterial(unsigned int skinIndex, unsigned int matIndex, const NzMaterial* material);
		void SetMesh(const NzMesh* mesh, const NzModelParameters& parameters = NzModelParameters());
		void SetSkinCount(unsigned int skinCount);
		bool SetSequence(const NzString& sequenceName);
		void SetSequence(unsigned int sequenceIndex);

		void Update(float elapsedTime);

	private:
		std::vector<const NzMaterial*> m_materials;
		NzSkeleton m_skeleton; // Uniquement pour les animations squelettiques
		const NzAnimation* m_animation;
		const NzMesh* m_mesh;
		const NzSequence* m_currentSequence;
		float m_interpolation;
		unsigned int m_currentFrame;
		unsigned int m_matCount;
		unsigned int m_nextFrame;
		unsigned int m_skinCount;
};

#endif // NAZARA_MODEL_HPP

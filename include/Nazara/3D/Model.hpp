// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MODEL_HPP
#define NAZARA_MODEL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/3D/SceneNode.hpp>
#include <Nazara/Core/Updatable.hpp>
#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Utility/Mesh.hpp>

struct NzModelParameters
{
	bool loadAnimation = true;
	bool loadMaterials = true;
	NzAnimationParams animation;
	NzMaterialParams material;
	NzMeshParams mesh;
};

class NAZARA_API NzModel : public NzSceneNode, public NzUpdatable
{
	friend class NzScene;

	public:
		NzModel();
		NzModel(const NzModel& model);
		~NzModel();

		void AddToRenderQueue(NzRenderQueue& renderQueue) const;
		void AdvanceAnimation(float elapsedTime);

		void EnableAnimation(bool animation);
		void EnableDraw(bool draw);

		NzAnimation* GetAnimation() const;
		const NzBoundingBoxf& GetBoundingBox() const;
		NzMaterial* GetMaterial(unsigned int matIndex) const;
		NzMaterial* GetMaterial(unsigned int skinIndex, unsigned int matIndex) const;
		unsigned int GetMaterialCount() const;
		unsigned int GetSkin() const;
		unsigned int GetSkinCount() const;
		NzMesh* GetMesh() const;
		nzSceneNodeType GetSceneNodeType() const override;
		NzSkeleton* GetSkeleton();
		const NzSkeleton* GetSkeleton() const;

		bool HasAnimation() const;

		bool IsAnimationEnabled() const;
		bool IsDrawEnabled() const;
		bool IsVisible(const NzFrustumf& frustum) const override;

		bool LoadFromFile(const NzString& meshPath, const NzModelParameters& modelParameters = NzModelParameters());
		bool LoadFromMemory(const void* data, std::size_t size, const NzModelParameters& modelParameters = NzModelParameters());
		bool LoadFromStream(NzInputStream& stream, const NzModelParameters& modelParameters = NzModelParameters());

		void Reset();

		bool SetAnimation(NzAnimation* animation);
		void SetMaterial(unsigned int matIndex, NzMaterial* material);
		void SetMaterial(unsigned int skinIndex, unsigned int matIndex, NzMaterial* material);
		void SetMesh(NzMesh* mesh, const NzModelParameters& parameters = NzModelParameters());
		void SetSkin(unsigned int skin);
		void SetSkinCount(unsigned int skinCount);
		bool SetSequence(const NzString& sequenceName);
		void SetSequence(unsigned int sequenceIndex);

	private:
		void Invalidate() override;
		void Register() override;
		void Unregister() override;
		void Update() override;
		void UpdateBoundingBox() const;

		std::vector<NzMaterial*> m_materials;
		mutable NzBoundingBoxf m_boundingBox;
		NzSkeleton m_skeleton; // Uniquement pour les animations squelettiques
		NzAnimation* m_animation;
		NzMesh* m_mesh;
		const NzSequence* m_currentSequence;
		bool m_animationEnabled;
		mutable bool m_boundingBoxUpdated;
		bool m_drawEnabled;
		float m_interpolation;
		unsigned int m_currentFrame;
		unsigned int m_matCount;
		unsigned int m_nextFrame;
		unsigned int m_skin;
		unsigned int m_skinCount;
};

#endif // NAZARA_MODEL_HPP

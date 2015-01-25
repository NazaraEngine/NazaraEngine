// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SKELETALMODEL_HPP
#define NAZARA_SKELETALMODEL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/Updatable.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <vector>

struct NAZARA_API NzSkeletalModelParameters : public NzModelParameters
{
	bool loadAnimation = true;
	NzAnimationParams animation;

	bool IsValid() const;
};

class NzSkeletalModel;

using NzSkeletalModelLoader = NzResourceLoader<NzSkeletalModel, NzSkeletalModelParameters>;

class NAZARA_API NzSkeletalModel : public NzModel, NzUpdatable
{
	friend NzSkeletalModelLoader;

	public:
		NzSkeletalModel();
		NzSkeletalModel(const NzSkeletalModel& model);
		NzSkeletalModel(NzSkeletalModel&& model);
		~NzSkeletalModel();

		void AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const override;
		void AdvanceAnimation(float elapsedTime);

		NzSkeletalModel* Clone() const;
		NzSkeletalModel* Create() const;

		void EnableAnimation(bool animation);

		NzAnimation* GetAnimation() const;
		NzSkeleton* GetSkeleton();
		const NzSkeleton* GetSkeleton() const;

		bool HasAnimation() const;

		bool IsAnimated() const;
		bool IsAnimationEnabled() const;
		bool IsDrawable() const;

		bool LoadFromFile(const NzString& filePath, const NzSkeletalModelParameters& params = NzSkeletalModelParameters());
		bool LoadFromMemory(const void* data, std::size_t size, const NzSkeletalModelParameters& params = NzSkeletalModelParameters());
		bool LoadFromStream(NzInputStream& stream, const NzSkeletalModelParameters& params = NzSkeletalModelParameters());

		void Reset();

		bool SetAnimation(NzAnimation* animation);
		void SetMesh(NzMesh* mesh) override;
		bool SetSequence(const NzString& sequenceName);
		void SetSequence(unsigned int sequenceIndex);

		NzSkeletalModel& operator=(const NzSkeletalModel& node);
		NzSkeletalModel& operator=(NzSkeletalModel&& node);

	private:
		void MakeBoundingVolume() const override;
		void Register() override;
		void Unregister() override;
		void Update() override;

		NzAnimationRef m_animation;
		NzSkeleton m_skeleton;
		const NzSequence* m_currentSequence;
		bool m_animationEnabled;
		float m_interpolation;
		unsigned int m_currentFrame;
		unsigned int m_nextFrame;

		static NzSkeletalModelLoader::LoaderList s_loaders;
};

#endif // NAZARA_SKELETALMODEL_HPP

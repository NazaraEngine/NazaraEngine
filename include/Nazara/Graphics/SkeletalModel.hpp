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

namespace Nz
{
	struct NAZARA_GRAPHICS_API SkeletalModelParameters : ModelParameters
	{
		bool loadAnimation = true;
		AnimationParams animation;

		bool IsValid() const;
	};

	class SkeletalModel;

	using SkeletalModelLoader = ResourceLoader<SkeletalModel, SkeletalModelParameters>;

	class NAZARA_GRAPHICS_API SkeletalModel : public Model, Updatable
	{
		friend SkeletalModelLoader;

		public:
			SkeletalModel();
			SkeletalModel(const SkeletalModel& model) = default;
			SkeletalModel(SkeletalModel&& model) = default;
			~SkeletalModel() = default;

			void AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData) const override;
			void AdvanceAnimation(float elapsedTime);

			SkeletalModel* Clone() const;
			SkeletalModel* Create() const;

			void EnableAnimation(bool animation);

			Animation* GetAnimation() const;
			Skeleton* GetSkeleton();
			const Skeleton* GetSkeleton() const;

			bool HasAnimation() const;

			bool IsAnimated() const override;
			bool IsAnimationEnabled() const;

			bool LoadFromFile(const String& filePath, const SkeletalModelParameters& params = SkeletalModelParameters());
			bool LoadFromMemory(const void* data, std::size_t size, const SkeletalModelParameters& params = SkeletalModelParameters());
			bool LoadFromStream(Stream& stream, const SkeletalModelParameters& params = SkeletalModelParameters());

			bool SetAnimation(Animation* animation);
			void SetMesh(Mesh* mesh) override;
			bool SetSequence(const String& sequenceName);
			void SetSequence(unsigned int sequenceIndex);

			SkeletalModel& operator=(const SkeletalModel& node) = default;
			SkeletalModel& operator=(SkeletalModel&& node) = default;

		private:
			void MakeBoundingVolume() const override;
			/*void Register() override;
			void Unregister() override;*/
			void Update() override;

			AnimationRef m_animation;
			Skeleton m_skeleton;
			const Sequence* m_currentSequence;
			bool m_animationEnabled;
			float m_interpolation;
			unsigned int m_currentFrame;
			unsigned int m_nextFrame;

			static SkeletalModelLoader::LoaderList s_loaders;
	};
}

#endif // NAZARA_SKELETALMODEL_HPP

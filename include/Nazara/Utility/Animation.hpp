// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ANIMATION_HPP
#define NAZARA_ANIMATION_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Sequence.hpp>
#include <limits>

namespace Nz
{
	struct NAZARA_UTILITY_API AnimationParams : ResourceParameters
	{
		// La frame de fin à charger
		unsigned int endFrame = std::numeric_limits<unsigned int>::max();
		// La frame de début à charger
		unsigned int startFrame = 0;

		bool IsValid() const;
	};

	class Animation;
	class Skeleton;

	using AnimationConstRef = ObjectRef<const Animation>;
	using AnimationLibrary = ObjectLibrary<Animation>;
	using AnimationLoader = ResourceLoader<Animation, AnimationParams>;
	using AnimationManager = ResourceManager<Animation, AnimationParams>;
	using AnimationRef = ObjectRef<Animation>;

	struct AnimationImpl;

	class NAZARA_UTILITY_API Animation : public RefCounted, public Resource
	{
		friend AnimationLibrary;
		friend AnimationLoader;
		friend AnimationManager;
		friend class Utility;

		public:
			Animation() = default;
			~Animation();

			bool AddSequence(const Sequence& sequence);
			void AnimateSkeleton(Skeleton* targetSkeleton, unsigned int frameA, unsigned int frameB, float interpolation) const;

			bool CreateSkeletal(unsigned int frameCount, unsigned int jointCount);
			void Destroy();

			void EnableLoopPointInterpolation(bool loopPointInterpolation);

			unsigned int GetFrameCount() const;
			unsigned int GetJointCount() const;
			Sequence* GetSequence(const String& sequenceName);
			Sequence* GetSequence(unsigned int index);
			const Sequence* GetSequence(const String& sequenceName) const;
			const Sequence* GetSequence(unsigned int index) const;
			unsigned int GetSequenceCount() const;
			int GetSequenceIndex(const String& sequenceName) const;
			SequenceJoint* GetSequenceJoints(unsigned int frameIndex = 0);
			const SequenceJoint* GetSequenceJoints(unsigned int frameIndex = 0) const;
			AnimationType GetType() const;

			bool HasSequence(const String& sequenceName) const;
			bool HasSequence(unsigned int index = 0) const;

			bool IsLoopPointInterpolationEnabled() const;
			bool IsValid() const;

			bool LoadFromFile(const String& filePath, const AnimationParams& params = AnimationParams());
			bool LoadFromMemory(const void* data, std::size_t size, const AnimationParams& params = AnimationParams());
			bool LoadFromStream(Stream& stream, const AnimationParams& params = AnimationParams());

			void RemoveSequence(const String& sequenceName);
			void RemoveSequence(unsigned int index);

			template<typename... Args> static AnimationRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnAnimationDestroy, const Animation* /*animation*/);
			NazaraSignal(OnAnimationRelease, const Animation* /*animation*/);

		private:
			static bool Initialize();
			static void Uninitialize();

			AnimationImpl* m_impl = nullptr;

			static AnimationLibrary::LibraryMap s_library;
			static AnimationLoader::LoaderList s_loaders;
			static AnimationManager::ManagerMap s_managerMap;
			static AnimationManager::ManagerParams s_managerParameters;
	};
}

#include <Nazara/Utility/Animation.inl>

#endif // NAZARA_ANIMATION_HPP

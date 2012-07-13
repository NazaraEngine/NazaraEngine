// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ANIMATION_HPP
#define NAZARA_ANIMATION_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Resource.hpp>
#include <Nazara/Utility/ResourceLoader.hpp>
#include <list>
#include <map>

struct NzAnimationParams
{
	unsigned int endFrame = static_cast<unsigned int>(-1);
	unsigned int startFrame = 0;

	bool IsValid() const;
};

struct NzSequence
{
	NzString name;
	unsigned int firstFrame;
	unsigned int lastFrame;
	unsigned int framePerSecond;
};

class NzAnimation;

typedef NzResourceLoader<NzAnimation, NzAnimationParams> NzAnimationLoader;

struct NzAnimationImpl;

class NAZARA_API NzAnimation : public NzResource
{
	friend class NzResourceLoader<NzAnimation, NzAnimationParams>;

	public:
		NzAnimation() = default;
		~NzAnimation();

		unsigned int AddSequence(const NzSequence& sequence);

		bool Create(nzAnimationType type, unsigned int frameCount);
		void Destroy();

		unsigned int GetFrameCount() const;
		NzSequence* GetSequence(const NzString& sequenceName);
		NzSequence* GetSequence(unsigned int index);
		const NzSequence* GetSequence(const NzString& sequenceName) const;
		const NzSequence* GetSequence(unsigned int index) const;
		unsigned int GetSequenceCount() const;
		nzAnimationType GetType() const;

		bool HasSequence(const NzString& sequenceName) const;
		bool HasSequence(unsigned int index = 0) const;

		bool IsValid() const;

		bool LoadFromFile(const NzString& filePath, const NzAnimationParams& params = NzAnimationParams());
		bool LoadFromMemory(const void* data, std::size_t size, const NzAnimationParams& params = NzAnimationParams());
		bool LoadFromStream(NzInputStream& stream, const NzAnimationParams& params = NzAnimationParams());

		void RemoveSequence(const NzString& sequenceName);
		void RemoveSequence(unsigned int index);

	private:
		NzAnimationImpl* m_impl = nullptr;

		static std::list<NzAnimationLoader::MemoryLoader> s_memoryLoaders;
		static std::list<NzAnimationLoader::StreamLoader> s_streamLoaders;
		static std::multimap<NzString, NzAnimationLoader::LoadFileFunction> s_fileLoaders;
};

#endif // NAZARA_ANIMATION_HPP

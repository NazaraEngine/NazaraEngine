// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOUNDBUFFER_HPP
#define NAZARA_SOUNDBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>

struct NzSoundBufferParams
{
	bool forceMono = false;

	bool IsValid() const;
};

class NzSound;
class NzSoundBuffer;

using NzSoundBufferConstListener = NzObjectListenerWrapper<const NzSoundBuffer>;
using NzSoundBufferConstRef = NzObjectRef<const NzSoundBuffer>;
using NzSoundBufferLibrary = NzObjectLibrary<NzSoundBuffer>;
using NzSoundBufferListener = NzObjectListenerWrapper<NzSoundBuffer>;
using NzSoundBufferLoader = NzResourceLoader<NzSoundBuffer, NzSoundBufferParams>;
using NzSoundBufferManager = NzResourceManager<NzSoundBuffer, NzSoundBufferParams>;
using NzSoundBufferRef = NzObjectRef<NzSoundBuffer>;

struct NzSoundBufferImpl;

class NAZARA_API NzSoundBuffer : public NzRefCounted, public NzResource, NzNonCopyable
{
	friend NzSound;
	friend NzSoundBufferLibrary;
	friend NzSoundBufferLoader;
	friend NzSoundBufferManager;
	friend class NzAudio;

	public:
		NzSoundBuffer() = default;
		NzSoundBuffer(nzAudioFormat format, unsigned int sampleCount, unsigned int sampleRate, const nzInt16* samples);
		~NzSoundBuffer();

		bool Create(nzAudioFormat format, unsigned int sampleCount, unsigned int sampleRate, const nzInt16* samples);
		void Destroy();

		nzUInt32 GetDuration() const;
		nzAudioFormat GetFormat() const;
		const nzInt16* GetSamples() const;
		unsigned int GetSampleCount() const;
		unsigned int GetSampleRate() const;

		bool IsValid() const;

		bool LoadFromFile(const NzString& filePath, const NzSoundBufferParams& params = NzSoundBufferParams());
		bool LoadFromMemory(const void* data, std::size_t size, const NzSoundBufferParams& params = NzSoundBufferParams());
		bool LoadFromStream(NzInputStream& stream, const NzSoundBufferParams& params = NzSoundBufferParams());

		static bool IsFormatSupported(nzAudioFormat format);
		template<typename... Args> static NzSoundBufferRef New(Args&&... args);

	private:
		unsigned int GetOpenALBuffer() const;

		static bool Initialize();
		static void Uninitialize();

		NzSoundBufferImpl* m_impl = nullptr;

		static NzSoundBufferLibrary::LibraryMap s_library;
		static NzSoundBufferLoader::LoaderList s_loaders;
		static NzSoundBufferManager::ManagerMap s_managerMap;
		static NzSoundBufferManager::ManagerParams s_managerParameters;
};

#include <Nazara/Audio/SoundBuffer.inl>

#endif // NAZARA_SOUNDBUFFER_HPP

// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOUNDBUFFER_HPP
#define NAZARA_SOUNDBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/Signal.hpp>

namespace Nz
{
	struct SoundBufferParams : ResourceParameters
	{
		bool forceMono = false;

		bool IsValid() const;
	};

	class Sound;
	class SoundBuffer;

	using SoundBufferConstRef = ObjectRef<const SoundBuffer>;
	using SoundBufferLibrary = ObjectLibrary<SoundBuffer>;
	using SoundBufferLoader = ResourceLoader<SoundBuffer, SoundBufferParams>;
	using SoundBufferManager = ResourceManager<SoundBuffer, SoundBufferParams>;
	using SoundBufferRef = ObjectRef<SoundBuffer>;

	struct SoundBufferImpl;

	class NAZARA_AUDIO_API SoundBuffer : public RefCounted, public Resource
	{
		friend Sound;
		friend SoundBufferLibrary;
		friend SoundBufferLoader;
		friend SoundBufferManager;
		friend class Audio;

		public:
			SoundBuffer() = default;
			SoundBuffer(AudioFormat format, UInt64 sampleCount, UInt32 sampleRate, const Int16* samples);
			SoundBuffer(const SoundBuffer&) = delete;
			SoundBuffer(SoundBuffer&&) = delete;
			~SoundBuffer();

			bool Create(AudioFormat format, UInt64 sampleCount, UInt32 sampleRate, const Int16* samples);
			void Destroy();

			UInt32 GetDuration() const;
			AudioFormat GetFormat() const;
			const Int16* GetSamples() const;
			UInt64 GetSampleCount() const;
			UInt32 GetSampleRate() const;

			bool IsValid() const;

			bool LoadFromFile(const String& filePath, const SoundBufferParams& params = SoundBufferParams());
			bool LoadFromMemory(const void* data, std::size_t size, const SoundBufferParams& params = SoundBufferParams());
			bool LoadFromStream(Stream& stream, const SoundBufferParams& params = SoundBufferParams());

			static bool IsFormatSupported(AudioFormat format);
			template<typename... Args> static SoundBufferRef New(Args&&... args);

			SoundBuffer& operator=(const SoundBuffer&) = delete;
			SoundBuffer& operator=(SoundBuffer&&) = delete; ///TODO

			// Signals:
			NazaraSignal(OnSoundBufferDestroy, const SoundBuffer* /*soundBuffer*/);
			NazaraSignal(OnSoundBufferRelease, const SoundBuffer* /*soundBuffer*/);

		private:
			unsigned int GetOpenALBuffer() const;

			static bool Initialize();
			static void Uninitialize();

			SoundBufferImpl* m_impl = nullptr;

			static SoundBufferLibrary::LibraryMap s_library;
			static SoundBufferLoader::LoaderList s_loaders;
			static SoundBufferManager::ManagerMap s_managerMap;
			static SoundBufferManager::ManagerParams s_managerParameters;
	};
}

#include <Nazara/Audio/SoundBuffer.inl>

#endif // NAZARA_SOUNDBUFFER_HPP

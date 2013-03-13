// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOUNDBUFFER_HPP
#define NAZARA_SOUNDBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>

struct NzSoundBufferParams
{
	bool IsValid() const;
};

class NzSound;
class NzSoundBuffer;

using NzSoundBufferLoader = NzResourceLoader<NzSoundBuffer, NzSoundBufferParams>;

struct NzSoundBufferImpl;

class NAZARA_API NzSoundBuffer : public NzResource, public NzNonCopyable
{
	friend NzSound;
	friend NzSoundBufferLoader;

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

	private:
		unsigned int GetOpenALBuffer() const;

		NzSoundBufferImpl* m_impl = nullptr;

		static NzSoundBufferLoader::LoaderList s_loaders;
};

#endif // NAZARA_SOUNDBUFFER_HPP

// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/SoundStream.hpp>

namespace Nz
{
	bool SoundStreamParams::IsValid() const
	{
		return true;
	}

	/*!
	* \ingroup audio
	* \class Nz::SoundStream
	* \brief Audio class that represents a sound stream
	*
	* \remark This class is abstract
	*/

	SoundStream::~SoundStream() = default;

	SoundStreamRef SoundStream::OpenFromFile(const std::filesystem::path& filePath, const SoundStreamParams& params)
	{
		return SoundStreamLoader::LoadFromFile(filePath, params);
	}

	SoundStreamRef SoundStream::OpenFromMemory(const void* data, std::size_t size, const SoundStreamParams& params)
	{
		return SoundStreamLoader::LoadFromMemory(data, size, params);
	}

	SoundStreamRef SoundStream::OpenFromStream(Stream& stream, const SoundStreamParams& params)
	{
		return SoundStreamLoader::LoadFromStream(stream, params);
	}

	SoundStreamLoader::LoaderList SoundStream::s_loaders;
}

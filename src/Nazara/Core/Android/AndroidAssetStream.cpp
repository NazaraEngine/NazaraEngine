// Copyright (C) 2025 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Android/AndroidAssetStream.hpp>
#include <NazaraUtils/Algorithm.hpp>

namespace Nz
{
	AndroidAssetStream::~AndroidAssetStream()
	{
		AAsset_close(m_asset);
	}

	UInt64 AndroidAssetStream::GetSize() const
	{
		return SafeCast<UInt64>(AAsset_getLength64(m_asset));
	}

	void AndroidAssetStream::FlushStream()
	{
		// nothing to do
	}

	std::size_t AndroidAssetStream::ReadBlock(void* buffer, std::size_t size)
	{
		// TODO: Handle the case where size > std::numeric_limits<int>::max()
		int byteRead = AAsset_read(m_asset, buffer, size);
		if (byteRead < 0)
		{
			NazaraError("AAsset_read failed (returned {})", byteRead);
			return 0;
		}

		return SafeCast<std::size_t>(byteRead);
	}

	bool AndroidAssetStream::SeekStreamCursor(UInt64 offset)
	{
		return AAsset_seek64(m_asset, SafeCast<off64_t>(offset), SEEK_SET) != -1;
	}

	UInt64 AndroidAssetStream::TellStreamCursor() const
	{
		return SafeCast<UInt64>(AAsset_getLength64(m_asset) - AAsset_getRemainingLength64(m_asset));
	}

	bool AndroidAssetStream::TestStreamEnd() const
	{
		return AAsset_getRemainingLength64(m_asset) == 0;
	}

	std::size_t AndroidAssetStream::WriteBlock(const void* /*buffer*/, std::size_t /*size*/)
	{
		NazaraError("AndroidAssetStream does not support write operations");
		return 0;
	}
}

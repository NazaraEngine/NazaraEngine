// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline void SoundDataReader::EnableLooping(bool loop)
	{
		m_isLooping.store(loop, std::memory_order_relaxed);
	}

	inline UInt64 SoundDataReader::GetReadOffset() const
	{
		return m_readOffset;
	}

	inline const std::shared_ptr<SoundDataSource>& SoundDataReader::GetSource()
	{
		return m_source;
	}

	inline bool SoundDataReader::IsLooping() const
	{
		return m_isLooping.load(std::memory_order_relaxed);
	}

	inline void SoundDataReader::UpdateReadOffset(UInt64 offset)
	{
		m_readOffset = offset;
	}

	inline bool SoundDataReader::UpdateReadOffset(UInt64 offset, UInt64 expectedPreviousValue)
	{
		return m_readOffset.compare_exchange_strong(expectedPreviousValue, offset);
	}
}

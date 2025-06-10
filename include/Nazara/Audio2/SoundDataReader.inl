// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline UInt64 SoundDataReader::GetReadOffset() const
	{
		return m_readOffset;
	}

	inline const std::shared_ptr<SoundDataSource>& SoundDataReader::GetSource()
	{
		return m_source;
	}

	inline void SoundDataReader::UpdateReadOffset(UInt64 offset)
	{
		m_readOffset = offset;
	}
}

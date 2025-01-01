// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	namespace Detail
	{
		inline bool SortByteArrayByCapacity(const ByteArray& byteArray, std::size_t refCapacity)
		{
			return refCapacity > byteArray.GetCapacity();
		}
	}

	inline void ByteArrayPool::Clear()
	{
		m_byteArrays.clear();
	}

	inline ByteArray ByteArrayPool::GetByteArray(std::size_t capacity)
	{
		ByteArray ret;

		auto it = std::lower_bound(m_byteArrays.begin(), m_byteArrays.end(), capacity, Detail::SortByteArrayByCapacity);
		if (it != m_byteArrays.end())
		{
			ret = std::move(*it);
			m_byteArrays.erase(it);
		}

		return ret;
	}

	inline void ByteArrayPool::ReturnByteArray(ByteArray byteArray)
	{
		auto it = std::lower_bound(m_byteArrays.begin(), m_byteArrays.end(), byteArray.GetCapacity(), Detail::SortByteArrayByCapacity);
		m_byteArrays.emplace(it, std::move(byteArray));
	}
}

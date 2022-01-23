// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/IndexIterator.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		UInt32 GetterSequential(const void* buffer, std::size_t i)
		{
			NazaraUnused(buffer);

			return static_cast<UInt32>(i);
		}

		UInt32 Getter16(const void* buffer, std::size_t i)
		{
			const UInt16* ptr = static_cast<const UInt16*>(buffer);
			return ptr[i];
		}

		UInt32 Getter32(const void* buffer, std::size_t i)
		{
			const UInt32* ptr = static_cast<const UInt32*>(buffer);
			return ptr[i];
		}

		UInt32 GetterError(const void*, std::size_t)
		{
			NazaraError("index buffer has no read flag");
			return 0;
		}

		void Setter16(void* buffer, std::size_t i, UInt32 value)
		{
			UInt16* ptr = static_cast<UInt16*>(buffer);
			ptr[i] = static_cast<UInt16>(value);
		}

		void Setter32(void* buffer, std::size_t i, UInt32 value)
		{
			UInt32* ptr = static_cast<UInt32*>(buffer);
			ptr[i] = value;
		}

		void SetterError(void*, std::size_t, UInt32)
		{
			NazaraError("index buffer has no write flag");
		}
	}

	IndexMapper::IndexMapper(IndexBuffer& indexBuffer, std::size_t indexCount) :
	m_indexCount((indexCount != 0) ? indexCount : indexBuffer.GetIndexCount())
	{
		if (!m_mapper.Map(indexBuffer, 0, m_indexCount))
			NazaraError("Failed to map buffer"); ///TODO: Unexcepted

		if (indexBuffer.GetBuffer()->GetUsageFlags().Test(BufferUsage::Read))
			m_getter = (indexBuffer.HasLargeIndices()) ? Getter32 : Getter16;
		else
			m_getter = GetterError;

		if (indexBuffer.GetBuffer()->GetUsageFlags().Test(BufferUsage::Write))
			m_setter = (indexBuffer.HasLargeIndices()) ? Setter32 : Setter16;
		else
			m_setter = SetterError;
	}

	IndexMapper::IndexMapper(SubMesh& subMesh) :
	IndexMapper(*subMesh.GetIndexBuffer(), (subMesh.GetIndexBuffer()) ? 0 : subMesh.GetVertexCount())
	{
	}

	UInt32 IndexMapper::Get(std::size_t i) const
	{
		NazaraAssert(i < m_indexCount, "Index out of range");

		return m_getter(m_mapper.GetPointer(), i);
	}

	const IndexBuffer* IndexMapper::GetBuffer() const
	{
		return m_mapper.GetBuffer();
	}

	std::size_t IndexMapper::GetIndexCount() const
	{
		return m_indexCount;
	}

	void IndexMapper::Set(std::size_t i, UInt32 value)
	{
		NazaraAssert(i < m_indexCount, "Index out of range");

		m_setter(m_mapper.GetPointer(), i, value);
	}

	void IndexMapper::Unmap()
	{
		m_mapper.Unmap();
	}

	IndexIterator IndexMapper::begin()
	{
		return {this, 0};
	}

	IndexIterator IndexMapper::end()
	{
		return {this, m_indexCount}; // Post-end
	}
}

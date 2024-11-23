// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/IndexMapper.hpp>
#include <Nazara/Core/IndexBuffer.hpp>
#include <Nazara/Core/IndexIterator.hpp>
#include <Nazara/Core/SubMesh.hpp>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		UInt32 Getter8(const void* buffer, std::size_t i)
		{
			const UInt8* ptr = static_cast<const UInt8*>(buffer);
			return ptr[i];
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

		void Setter8(void* buffer, std::size_t i, UInt32 value)
		{
			UInt8* ptr = static_cast<UInt8*>(buffer);
			ptr[i] = SafeCast<UInt8>(value);
		}

		void Setter16(void* buffer, std::size_t i, UInt32 value)
		{
			UInt16* ptr = static_cast<UInt16*>(buffer);
			ptr[i] = SafeCast<UInt16>(value);
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

	IndexMapper::IndexMapper(IndexBuffer& indexBuffer, UInt32 indexCount) :
	m_indexCount((indexCount != 0) ? indexCount : indexBuffer.GetIndexCount())
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		if (!m_mapper.Map(indexBuffer, 0, m_indexCount))
			NazaraError("failed to map buffer"); ///TODO: Unexcepted

		Getter rightGetter = nullptr;
		Setter rightSetter = nullptr;

		switch (indexBuffer.GetIndexType())
		{
			case IndexType::U8:
				rightGetter = Getter8;
				rightSetter = Setter8;
				break;

			case IndexType::U16:
				rightGetter = Getter16;
				rightSetter = Setter16;
				break;

			case IndexType::U32:
				rightGetter = Getter32;
				rightSetter = Setter32;
				break;
		}

		if (!rightGetter)
			NazaraError("unexpected index size"); ///TODO: Unexcepted

		if (indexBuffer.GetBuffer()->GetUsageFlags().Test(BufferUsage::Read))
			m_getter = rightGetter;
		else
			m_getter = GetterError;

		if (indexBuffer.GetBuffer()->GetUsageFlags().Test(BufferUsage::Write))
			m_setter = rightSetter;
		else
			m_setter = SetterError;
	}

	IndexMapper::IndexMapper(SubMesh& subMesh) :
	IndexMapper(*subMesh.GetIndexBuffer(), (subMesh.GetIndexBuffer()) ? 0 : subMesh.GetVertexCount())
	{
	}

	UInt32 IndexMapper::Get(std::size_t i) const
	{
		NazaraAssertMsg(i < m_indexCount, "Index out of range");

		return m_getter(m_mapper.GetPointer(), i);
	}

	const IndexBuffer* IndexMapper::GetBuffer() const
	{
		return m_mapper.GetBuffer();
	}

	void IndexMapper::Set(std::size_t i, UInt32 value)
	{
		NazaraAssertMsg(i < m_indexCount, "Index out of range");

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

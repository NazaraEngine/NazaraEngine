// Copyright (C) 2017 Jérôme Leclercq
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
		UInt32 GetterSequential(const void* buffer, unsigned int i)
		{
			NazaraUnused(buffer);

			return i;
		}

		UInt32 Getter16(const void* buffer, unsigned int i)
		{
			const UInt16* ptr = static_cast<const UInt16*>(buffer);
			return ptr[i];
		}

		UInt32 Getter32(const void* buffer, unsigned int i)
		{
			const UInt32* ptr = static_cast<const UInt32*>(buffer);
			return ptr[i];
		}

		void Setter16(void* buffer, unsigned int i, UInt32 value)
		{
			UInt16* ptr = static_cast<UInt16*>(buffer);
			ptr[i] = static_cast<UInt16>(value);
		}

		void Setter32(void* buffer, unsigned int i, UInt32 value)
		{
			UInt32* ptr = static_cast<UInt32*>(buffer);
			ptr[i] = value;
		}

		void SetterError(void*, unsigned int, UInt32)
		{
			NazaraError("Index buffer opened with read-only access");
		}
	}

	IndexMapper::IndexMapper(IndexBuffer* indexBuffer, BufferAccess access, std::size_t indexCount) :
	m_indexCount((indexCount != 0) ? indexCount : indexBuffer->GetIndexCount())
	{
		NazaraAssert(indexCount != 0 || indexBuffer, "Invalid index count with invalid index buffer");

		if (indexBuffer)
		{
			if (!m_mapper.Map(indexBuffer, access))
				NazaraError("Failed to map buffer"); ///TODO: Unexcepted

			if (indexBuffer->HasLargeIndices())
			{
				m_getter = Getter32;
				if (access != BufferAccess_ReadOnly)
					m_setter = Setter32;
				else
					m_setter = SetterError;
			}
			else
			{
				m_getter = Getter16;
				if (access != BufferAccess_ReadOnly)
					m_setter = Setter16;
				else
					m_setter = SetterError;
			}
		}
		else
		{
			m_getter = GetterSequential;
			m_setter = SetterError;
		}
	}

	IndexMapper::IndexMapper(SubMesh* subMesh, BufferAccess access) :
	IndexMapper(subMesh->GetIndexBuffer(), access, (subMesh->GetIndexBuffer()) ? 0 : subMesh->GetVertexCount())
	{
	}

	IndexMapper::IndexMapper(const IndexBuffer* indexBuffer, BufferAccess access, std::size_t indexCount) :
	m_setter(SetterError),
	m_indexCount((indexCount != 0) ? indexCount : indexBuffer->GetIndexCount())
	{
		NazaraAssert(indexCount != 0 || indexBuffer, "Invalid index count with invalid index buffer");

		if (indexBuffer)
		{
			if (!m_mapper.Map(indexBuffer, access))
				NazaraError("Failed to map buffer"); ///TODO: Unexcepted

			if (indexBuffer->HasLargeIndices())
				m_getter = Getter32;
			else
				m_getter = Getter16;
		}
		else
			m_getter = GetterSequential;
	}

	IndexMapper::IndexMapper(const SubMesh* subMesh, BufferAccess access) :
	IndexMapper(subMesh->GetIndexBuffer(), access, (subMesh->GetIndexBuffer()) ? 0 : subMesh->GetVertexCount())
	{
	}

	UInt32 IndexMapper::Get(unsigned int i) const
	{
		#if NAZARA_UTILITY_SAFE
		if (i >= m_indexCount)
		{
			NazaraError("Index out of range (" + String::Number(i) + " >= " + String::Number(m_indexCount) + ')');
			return 0;
		}
		#endif

		return m_getter(m_mapper.GetPointer(), i);
	}

	const IndexBuffer* IndexMapper::GetBuffer() const
	{
		return m_mapper.GetBuffer();
	}

	unsigned int IndexMapper::GetIndexCount() const
	{
		return m_indexCount;
	}

	void IndexMapper::Set(unsigned int i, UInt32 value)
	{
		#if NAZARA_UTILITY_SAFE
		if (i >= m_indexCount)
		{
			NazaraError("Index out of range (" + String::Number(i) + " >= " + String::Number(m_indexCount) + ')');
			return;
		}
		#endif

		m_setter(m_mapper.GetPointer(), i, value);
	}

	void IndexMapper::Unmap()
	{
		m_mapper.Unmap();
	}

	IndexIterator IndexMapper::begin()
	{
		return IndexIterator(this, 0);
	}

	IndexIterator IndexMapper::end()
	{
		return IndexIterator(this, m_indexCount); // Post-end
	}
}

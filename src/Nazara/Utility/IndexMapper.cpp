// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	nzUInt32 Getter16(const void* buffer, unsigned int i)
	{
		const nzUInt16* ptr = reinterpret_cast<const nzUInt16*>(buffer);
		return ptr[i];
	}

	nzUInt32 Getter32(const void* buffer, unsigned int i)
	{
		const nzUInt32* ptr = reinterpret_cast<const nzUInt32*>(buffer);
		return ptr[i];
	}

	void Setter16(void* buffer, unsigned int i, nzUInt32 value)
	{
		nzUInt16* ptr = reinterpret_cast<nzUInt16*>(buffer);
		ptr[i] = static_cast<nzUInt16>(value);
	}

	void Setter32(void* buffer, unsigned int i, nzUInt32 value)
	{
		nzUInt32* ptr = reinterpret_cast<nzUInt32*>(buffer);
		ptr[i] = value;
	}

	void SetterError(void*, unsigned int, nzUInt32)
	{
		NazaraError("Index buffer opened with read-only access");
	}
}

NzIndexMapper::NzIndexMapper(NzIndexBuffer* indexBuffer, nzBufferAccess access) :
m_mapper(indexBuffer, access)
{
	if (indexBuffer->HasLargeIndices())
	{
		m_getter = Getter32;
		if (access != nzBufferAccess_ReadOnly)
			m_setter = Setter32;
		else
			m_setter = SetterError;
	}
	else
	{
		m_getter = Getter16;
		if (access != nzBufferAccess_ReadOnly)
			m_setter = Setter16;
		else
			m_setter = SetterError;
	}
}

NzIndexMapper::NzIndexMapper(const NzIndexBuffer* indexBuffer) :
m_mapper(indexBuffer, nzBufferAccess_ReadOnly)
{
	if (indexBuffer->HasLargeIndices())
	{
		m_getter = Getter32;
		m_setter = SetterError;
	}
	else
	{
		m_getter = Getter16;
		m_setter = SetterError;
	}
}

NzIndexMapper::NzIndexMapper(const NzSubMesh* subMesh) :
NzIndexMapper(subMesh->GetIndexBuffer())
{
}

nzUInt32 NzIndexMapper::Get(unsigned int i) const
{
	return m_getter(m_mapper.GetPointer(), i);
}

void NzIndexMapper::Set(unsigned int i, nzUInt32 value)
{
	m_setter(m_mapper.GetPointer(), i, value);
}

void NzIndexMapper::Unmap()
{
	m_mapper.Unmap();
}

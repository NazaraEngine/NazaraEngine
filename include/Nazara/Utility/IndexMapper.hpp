// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INDEXMAPPER_HPP
#define NAZARA_INDEXMAPPER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/BufferMapper.hpp>

class NzIndexBuffer;
class NzSubMesh;

using NzIndexMapperGetter = nzUInt32 (*)(const void* buffer, unsigned int i);
using NzIndexMapperSetter = void (*)(void* buffer, unsigned int i, nzUInt32 value);

class NAZARA_API NzIndexMapper
{
	public:
		NzIndexMapper(NzIndexBuffer* indexBuffer, nzBufferAccess access = nzBufferAccess_ReadWrite);
		NzIndexMapper(const NzIndexBuffer* indexBuffer, nzBufferAccess access = nzBufferAccess_ReadOnly);
		NzIndexMapper(const NzSubMesh* subMesh);
		~NzIndexMapper() = default;

		nzUInt32 Get(unsigned int i) const;
		const NzIndexBuffer* GetBuffer() const;

		void Set(unsigned int i, nzUInt32 value);

		void Unmap();

	private:
		NzBufferMapper<NzIndexBuffer> m_mapper;
		NzIndexMapperGetter m_getter;
		NzIndexMapperSetter m_setter;
};

#endif // NAZARA_INDEXMAPPER_HPP

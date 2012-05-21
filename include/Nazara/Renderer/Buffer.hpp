// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BUFFER_HPP
#define NAZARA_BUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/NonCopyable.hpp>
#include <Nazara/Utility/Resource.hpp>

enum nzBufferLock
{
	nzBufferLock_DiscardAndWrite,
	nzBufferLock_ReadOnly,
	nzBufferLock_ReadWrite,
	nzBufferLock_WriteOnly
};

enum nzBufferStorage
{
	nzBufferStorage_Hardware,
	nzBufferStorage_Software
};

enum nzBufferType
{
	nzBufferType_Index,
	nzBufferType_Vertex
};

enum nzBufferUsage
{
	nzBufferUsage_Dynamic,
	nzBufferUsage_Static
};

class NzBufferImpl;
class NzRenderer;

class NAZARA_API NzBuffer : public NzResource, NzNonCopyable
{
	friend class NzRenderer;

	public:
		NzBuffer(nzBufferType type);
		NzBuffer(nzBufferType type, unsigned int length, nzUInt8 typeSize, nzBufferUsage usage = nzBufferUsage_Static);
		~NzBuffer();

		bool CopyContent(NzBuffer& buffer);

		bool Create(unsigned int length, nzUInt8 typeSize, nzBufferUsage usage = nzBufferUsage_Static);
		void Destroy();

		bool Fill(const void* data, unsigned int offset, unsigned int length);

		void* GetBufferPtr();
		const void* GetBufferPtr() const;
		NzBufferImpl* GetImpl() const;
		unsigned int GetLength() const;
		unsigned int GetSize() const;
		nzBufferStorage GetStorage() const;
		nzBufferType GetType() const;
		nzUInt8 GetTypeSize() const;
		nzBufferUsage GetUsage() const;

		bool IsHardware() const;

		void* Lock(nzBufferLock lock, unsigned int offset = 0, unsigned int length = 0);
		bool Unlock();

		static bool IsSupported(nzBufferStorage storage);

	private:
		nzBufferStorage m_storage;
		nzBufferType m_type;
		nzBufferUsage m_usage;
		nzUInt8 m_typeSize;
		NzBufferImpl* m_impl;
		unsigned int m_length;

};

#endif // NAZARA_BUFFER_HPP

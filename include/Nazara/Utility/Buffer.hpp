// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BUFFER_HPP
#define NAZARA_BUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Utility/Enums.hpp>

class NzBufferImpl;
class NzRenderer;
class NzUtility;

class NAZARA_API NzBuffer : public NzResource, NzNonCopyable
{
	friend class NzRenderer;
	friend class NzUtility;

	public:
		typedef NzBufferImpl* (*BufferFunction)(NzBuffer* parent, nzBufferType type);

		NzBuffer(nzBufferType type);
		NzBuffer(nzBufferType type, unsigned int length, nzUInt8 typeSize, nzBufferStorage storage = nzBufferStorage_Software, nzBufferUsage usage = nzBufferUsage_Static);
		~NzBuffer();

		bool CopyContent(NzBuffer& buffer);

		bool Create(unsigned int length, nzUInt8 typeSize, nzBufferStorage storage = nzBufferStorage_Software, nzBufferUsage usage = nzBufferUsage_Static);
		void Destroy();

		bool Fill(const void* data, unsigned int offset, unsigned int length);

		NzBufferImpl* GetImpl() const;
		unsigned int GetLength() const;
		void* GetPointer();
		const void* GetPointer() const;
		unsigned int GetSize() const;
		nzBufferStorage GetStorage() const;
		nzBufferType GetType() const;
		nzUInt8 GetTypeSize() const;
		nzBufferUsage GetUsage() const;

		bool IsHardware() const;
		bool IsValid() const;

		void* Map(nzBufferAccess access, unsigned int offset = 0, unsigned int length = 0);

		bool SetStorage(nzBufferStorage storage);

		bool Unmap();

		static bool IsSupported(nzBufferStorage storage);
		static void SetBufferFunction(nzBufferStorage storage, BufferFunction func);

	private:
		static bool Initialize();
		static void Uninitialize();

		nzBufferStorage m_storage;
		nzBufferType m_type;
		nzBufferUsage m_usage;
		nzUInt8 m_typeSize;
		NzBufferImpl* m_impl;
		unsigned int m_length;

		static BufferFunction s_bufferFunctions[nzBufferStorage_Max+1];
};

#endif // NAZARA_BUFFER_HPP

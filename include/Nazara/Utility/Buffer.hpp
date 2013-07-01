// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BUFFER_HPP
#define NAZARA_BUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceRef.hpp>
#include <Nazara/Utility/Enums.hpp>

class NzBuffer;

using NzBufferConstRef = NzResourceRef<const NzBuffer>;
using NzBufferRef = NzResourceRef<NzBuffer>;

class NzAbstractBuffer;

class NAZARA_API NzBuffer : public NzResource, NzNonCopyable
{
	friend class NzUtility;

	public:
		using BufferFunction = NzAbstractBuffer* (*)(NzBuffer* parent, nzBufferType type);

		NzBuffer(nzBufferType type);
		NzBuffer(nzBufferType type, unsigned int size, nzBufferStorage storage = nzBufferStorage_Software, nzBufferUsage usage = nzBufferUsage_Static);
		~NzBuffer();

		bool CopyContent(const NzBuffer& buffer);

		bool Create(unsigned int size, nzBufferStorage storage = nzBufferStorage_Software, nzBufferUsage usage = nzBufferUsage_Static);
		void Destroy();

		bool Fill(const void* data, unsigned int offset, unsigned int size, bool forceDiscard = false);

		NzAbstractBuffer* GetImpl() const;
		unsigned int GetSize() const;
		nzBufferStorage GetStorage() const;
		nzBufferType GetType() const;
		nzBufferUsage GetUsage() const;

		bool IsHardware() const;
		bool IsValid() const;

		void* Map(nzBufferAccess access, unsigned int offset = 0, unsigned int size = 0);
		void* Map(nzBufferAccess access, unsigned int offset = 0, unsigned int size = 0) const;

		bool SetStorage(nzBufferStorage storage);

		void Unmap() const;

		static bool IsSupported(nzBufferStorage storage);
		static void SetBufferFunction(nzBufferStorage storage, BufferFunction func);

	private:
		static bool Initialize();
		static void Uninitialize();

		nzBufferStorage m_storage;
		nzBufferType m_type;
		nzBufferUsage m_usage;
		NzAbstractBuffer* m_impl;
		unsigned int m_size;

		static BufferFunction s_bufferFunctions[nzBufferStorage_Max+1];
};

#endif // NAZARA_BUFFER_HPP

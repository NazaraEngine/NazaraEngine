// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOFTWAREBUFFER_HPP
#define NAZARA_SOFTWAREBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/BufferImpl.hpp>

class NzSoftwareBuffer : public NzBufferImpl
{
	public:
		NzSoftwareBuffer(NzBuffer* parent, nzBufferType type);
		~NzSoftwareBuffer();

		void Bind();

		bool Create(unsigned int length, nzUInt8 typeSize, nzBufferUsage usage = nzBufferUsage_Static);
		void Destroy();

		bool Fill(const void* data, unsigned int offset, unsigned int length);

		bool IsHardware() const;

		void* Lock(nzBufferLock lock, unsigned int offset = 0, unsigned int length = 0);
		bool Unlock();

	private:
		nzBufferType m_type;
		nzUInt8 m_typeSize;
		nzUInt8* m_buffer;
		bool m_locked;
		unsigned int m_length;
};

#endif // NAZARA_SOFTWAREBUFFER_HPP

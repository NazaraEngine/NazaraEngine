// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOFTWAREBUFFER_HPP
#define NAZARA_SOFTWAREBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/BufferImpl.hpp>

class NzSoftwareBuffer : public NzBufferImpl
{
	public:
		NzSoftwareBuffer(NzBuffer* parent, nzBufferType type);
		~NzSoftwareBuffer();

		bool Create(unsigned int size, nzBufferUsage usage = nzBufferUsage_Static);
		void Destroy();

		bool Fill(const void* data, unsigned int offset, unsigned int size, bool forceDiscard);

		void* GetPointer();

		bool IsHardware() const;

		void* Map(nzBufferAccess access, unsigned int offset = 0, unsigned int size = 0);
		bool Unmap();

	private:
		nzBufferType m_type;
		nzUInt8* m_buffer;
		bool m_mapped;
};

#endif // NAZARA_SOFTWAREBUFFER_HPP

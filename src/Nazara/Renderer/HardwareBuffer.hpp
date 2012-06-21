// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HARDWAREBUFFER_HPP
#define NAZARA_HARDWAREBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Utility/BufferImpl.hpp>

class NzHardwareBuffer : public NzBufferImpl
{
	public:
		NzHardwareBuffer(NzBuffer* parent, nzBufferType type);
		~NzHardwareBuffer();

		void Bind();

		bool Create(unsigned int size, nzBufferUsage usage = nzBufferUsage_Static);
		void Destroy();

		bool Fill(const void* data, unsigned int offset, unsigned int length);

		void* GetPointer();

		bool IsHardware() const;

		void* Map(nzBufferAccess access, unsigned int offset = 0, unsigned int length = 0);
		bool Unmap();

	private:
		GLuint m_buffer;
		nzBufferType m_type;
		NzBuffer* m_parent;
};

#endif // NAZARA_HARDWAREBUFFER_HPP

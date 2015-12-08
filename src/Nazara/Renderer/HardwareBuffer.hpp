// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HARDWAREBUFFER_HPP
#define NAZARA_HARDWAREBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>

namespace Nz
{
	class HardwareBuffer : public AbstractBuffer
	{
		public:
			HardwareBuffer(Buffer* parent, BufferType type);
			~HardwareBuffer();

			bool Create(unsigned int size, BufferUsage usage = BufferUsage_Static);
			void Destroy();

			bool Fill(const void* data, unsigned int offset, unsigned int size, bool forceDiscard);

			bool IsHardware() const;

			void* Map(BufferAccess access, unsigned int offset = 0, unsigned int size = 0);
			bool Unmap();

			// Fonctions OpenGL
			void Bind() const;
			unsigned int GetOpenGLID() const;

		private:
			GLuint m_buffer;
			BufferType m_type;
			Buffer* m_parent;
	};
}

#endif // NAZARA_HARDWAREBUFFER_HPP

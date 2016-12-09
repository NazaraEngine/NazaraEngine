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
	class Buffer;

	class HardwareBuffer : public AbstractBuffer
	{
		public:
			HardwareBuffer(Buffer* parent, BufferType type);
			~HardwareBuffer();

			bool Fill(const void* data, UInt32 offset, UInt32 size) override;

			bool Initialize(unsigned int size, BufferUsageFlags usage) override;

			DataStorage GetStorage() const override;

			void* Map(BufferAccess access, UInt32 offset = 0, UInt32 size = 0) override;
			bool Unmap() override;

			// Fonctions OpenGL
			void Bind() const;
			GLuint GetOpenGLID() const;

		private:
			GLuint m_buffer;
			BufferType m_type;
			Buffer* m_parent;
	};
}

#endif // NAZARA_HARDWAREBUFFER_HPP

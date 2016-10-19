// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOFTWAREBUFFER_HPP
#define NAZARA_SOFTWAREBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>

namespace Nz
{
	class SoftwareBuffer : public AbstractBuffer
	{
		public:
			SoftwareBuffer(Buffer* parent, BufferType type);
			~SoftwareBuffer();

			bool Create(unsigned int size, BufferUsage usage = BufferUsage_Static);
			void Destroy();

			bool Fill(const void* data, unsigned int offset, unsigned int size, bool forceDiscard);

			bool IsHardware() const;

			void* Map(BufferAccess access, unsigned int offset = 0, unsigned int size = 0);
			bool Unmap();

		private:
			UInt8* m_buffer;
			bool m_mapped;
	};
}

#endif // NAZARA_SOFTWAREBUFFER_HPP

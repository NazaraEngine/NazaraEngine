// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOFTWAREBUFFER_HPP
#define NAZARA_SOFTWAREBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <vector>

namespace Nz
{
	class Buffer;

	class NAZARA_UTILITY_API SoftwareBuffer : public AbstractBuffer
	{
		public:
			SoftwareBuffer(Buffer* parent, BufferType type);
			~SoftwareBuffer();

			bool Fill(const void* data, UInt32 offset, UInt32 size) override;

			bool Initialize(UInt32 size, BufferUsageFlags usage) override;

			DataStorage GetStorage() const override;

			void* Map(BufferAccess access, UInt32 offset = 0, UInt32 size = 0) override;
			bool Unmap() override;

		private:
			std::vector<UInt8> m_buffer;
			bool m_mapped;
	};
}

#endif // NAZARA_SOFTWAREBUFFER_HPP

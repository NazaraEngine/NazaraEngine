// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_SOFTWAREBUFFER_HPP
#define NAZARA_UTILITY_SOFTWAREBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <vector>

namespace Nz
{
	class Buffer;

	class NAZARA_UTILITY_API SoftwareBuffer : public AbstractBuffer
	{
		public:
			SoftwareBuffer(Buffer* parent, BufferType type);
			~SoftwareBuffer() = default;

			bool Fill(const void* data, UInt64 offset, UInt64 size) override;

			bool Initialize(UInt64 size, BufferUsageFlags usage) override;

			const UInt8* GetData() const;
			UInt64 GetSize() const override;
			DataStorage GetStorage() const override;

			void* Map(BufferAccess access, UInt64 offset = 0, UInt64 size = 0) override;
			bool Unmap() override;

		private:
			std::vector<UInt8> m_buffer;
			bool m_mapped;
	};
}

#endif // NAZARA_UTILITY_SOFTWAREBUFFER_HPP

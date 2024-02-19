// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_SOFTWAREBUFFER_HPP
#define NAZARA_CORE_SOFTWAREBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Buffer.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_CORE_API SoftwareBuffer : public Buffer
	{
		public:
			SoftwareBuffer(BufferType type, UInt64 size, BufferUsageFlags usage, const void* initialData);
			~SoftwareBuffer() = default;

			bool Fill(const void* data, UInt64 offset, UInt64 size) override;

			const UInt8* GetData() const;

			void* Map(UInt64 offset = 0, UInt64 size = 0) override;
			bool Unmap() override;

		private:
			std::unique_ptr<UInt8[]> m_buffer;
			bool m_mapped;
	};

	NAZARA_CORE_API std::shared_ptr<Buffer> SoftwareBufferFactory(BufferType type, UInt64 size, BufferUsageFlags usage, const void* initialData = nullptr);
}

#endif // NAZARA_CORE_SOFTWAREBUFFER_HPP

// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_MEMORYVIEW_HPP
#define NAZARA_CORE_MEMORYVIEW_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Stream.hpp>

namespace Nz
{
	class NAZARA_CORE_API MemoryView : public Stream
	{
		public:
			MemoryView(void* ptr, UInt64 size);
			MemoryView(const void* ptr, UInt64 size);
			MemoryView(const MemoryView&) = delete;
			MemoryView(MemoryView&&) noexcept = default;
			~MemoryView() = default;

			UInt64 GetSize() const override;

			MemoryView& operator=(const MemoryView&) = delete;
			MemoryView& operator=(MemoryView&&) noexcept = default;

		private:
			void FlushStream() override;
			void* GetMemoryMappedPointer() const override;
			std::size_t ReadBlock(void* buffer, std::size_t size) override;
			bool SeekStreamCursor(UInt64 offset) override;
			bool TestStreamEnd() const override;
			UInt64 TellStreamCursor() const override;
			std::size_t WriteBlock(const void* buffer, std::size_t size) override;

			UInt8* m_ptr;
			UInt64 m_pos;
			UInt64 m_size;
	};
}

#endif // NAZARA_CORE_MEMORYVIEW_HPP

// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MEMORYVIEW_HPP
#define NAZARA_MEMORYVIEW_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Stream.hpp>

namespace Nz
{
	class NAZARA_CORE_API MemoryView : public Stream
	{
		public:
			MemoryView(void* ptr, UInt64 size);
			MemoryView(const void* ptr, UInt64 size);
			MemoryView(const MemoryView&) = delete;
			MemoryView(MemoryView&&) = delete; ///TODO
			~MemoryView() = default;

			bool EndOfStream() const override;

			UInt64 GetCursorPos() const override;
			UInt64 GetSize() const override;

			bool SetCursorPos(UInt64 offset) override;

			MemoryView& operator=(const MemoryView&) = delete;
			MemoryView& operator=(MemoryView&&) = delete; ///TODO

		private:
			void FlushStream() override;
			std::size_t ReadBlock(void* buffer, std::size_t size) override;
			std::size_t WriteBlock(const void* buffer, std::size_t size) override;

			UInt8* m_ptr;
			UInt64 m_pos;
			UInt64 m_size;
	};
}

#endif // NAZARA_MEMORYVIEW_HPP

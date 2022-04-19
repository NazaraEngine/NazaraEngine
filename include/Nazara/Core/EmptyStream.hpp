// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_EMPTYSTREAM_HPP
#define NAZARA_CORE_EMPTYSTREAM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Stream.hpp>

namespace Nz
{
	class NAZARA_CORE_API EmptyStream : public Stream
	{
		public:
			inline EmptyStream();
			EmptyStream(const EmptyStream&) = default;
			EmptyStream(EmptyStream&&) noexcept = default;
			~EmptyStream() = default;

			void Clear();

			UInt64 GetSize() const override;

			EmptyStream& operator=(const EmptyStream&) = default;
			EmptyStream& operator=(EmptyStream&&) noexcept = default;

		private:
			void FlushStream() override;
			std::size_t ReadBlock(void* buffer, std::size_t size) override;
			bool SeekStreamCursor(UInt64 offset) override;
			UInt64 TellStreamCursor() const override;
			bool TestStreamEnd() const override;
			std::size_t WriteBlock(const void* buffer, std::size_t size) override;

			UInt64 m_size;
	};
}

#include <Nazara/Core/EmptyStream.inl>

#endif // NAZARA_CORE_EMPTYSTREAM_HPP

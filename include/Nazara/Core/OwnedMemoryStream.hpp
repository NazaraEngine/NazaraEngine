// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_OWNEDMEMORYSTREAM_HPP
#define NAZARA_CORE_OWNEDMEMORYSTREAM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/MemoryStream.hpp>

namespace Nz
{
	class NAZARA_CORE_API OwnedMemoryStream : public MemoryStream
	{
		public:
			inline OwnedMemoryStream(OpenModeFlags openMode = OpenMode_ReadWrite);
			inline OwnedMemoryStream(ByteArray byteArray, OpenModeFlags openMode = OpenMode_ReadWrite);
			OwnedMemoryStream(const OwnedMemoryStream&) = delete;
			OwnedMemoryStream(OwnedMemoryStream&&) noexcept = default;
			~OwnedMemoryStream() = default;

			OwnedMemoryStream& operator=(const OwnedMemoryStream&) = delete;
			OwnedMemoryStream& operator=(OwnedMemoryStream&&) noexcept = default;

		private:
			using MemoryStream::SetBuffer;

			ByteArray m_ownedByteArray;
	};
}

#include <Nazara/Core/OwnedMemoryStream.inl>

#endif // NAZARA_CORE_OWNEDMEMORYSTREAM_HPP

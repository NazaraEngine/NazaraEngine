// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_HASHDIGEST_HPP
#define NAZARA_HASHDIGEST_HPP

///TODO: Remplacer par ByteArray

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <iosfwd>

namespace Nz
{
	class NAZARA_CORE_API HashDigest
	{
		public:
			HashDigest();
			HashDigest(const String& hashName, const UInt8* digest, unsigned int length);
			HashDigest(const HashDigest& rhs);
			HashDigest(HashDigest&& rhs) noexcept;
			~HashDigest();

			bool IsValid() const;

			const UInt8* GetDigest() const;
			unsigned int GetDigestLength() const;
			String GetHashName() const;

			String ToHex() const;

			UInt8 operator[](unsigned int pos) const;

			HashDigest& operator=(const HashDigest& rhs);
			HashDigest& operator=(HashDigest&& rhs) noexcept;

			bool operator==(const HashDigest& rhs) const;
			bool operator!=(const HashDigest& rhs) const;
			bool operator<(const HashDigest& rhs) const;
			bool operator<=(const HashDigest& rhs) const;
			bool operator>(const HashDigest& rhs) const;
			bool operator>=(const HashDigest& rhs) const;

			NAZARA_CORE_API friend std::ostream& operator<<(std::ostream& out, const HashDigest& string);

		private:
			String m_hashName;
			UInt8* m_digest;
			unsigned int m_digestLength;
	};
}

#endif // NAZARA_HASHDIGEST_HPP

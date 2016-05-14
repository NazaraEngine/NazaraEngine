// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BITSET_HPP
#define NAZARA_BITSET_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <limits>
#include <memory>
#include <type_traits>

namespace Nz
{
	class AbstractHash;

	template<typename Block = UInt32, class Allocator = std::allocator<Block>>
	class Bitset
	{
		static_assert(std::is_integral<Block>::value && std::is_unsigned<Block>::value, "Block must be a unsigned integral type");

		public:
			class Bit;

			Bitset();
			explicit Bitset(unsigned int bitCount, bool val);
			explicit Bitset(const char* bits);
			Bitset(const char* bits, unsigned int bitCount);
			Bitset(const Bitset& bitset) = default;
			explicit Bitset(const String& bits);
			template<typename T> Bitset(T value);
			Bitset(Bitset&& bitset) noexcept = default;
			~Bitset() noexcept = default;

			void Clear() noexcept;
			unsigned int Count() const;
			void Flip();

			unsigned int FindFirst() const;
			unsigned int FindNext(unsigned int bit) const;

			Block GetBlock(unsigned int i) const;
			unsigned int GetBlockCount() const;
			unsigned int GetCapacity() const;
			unsigned int GetSize() const;

			void PerformsAND(const Bitset& a, const Bitset& b);
			void PerformsNOT(const Bitset& a);
			void PerformsOR(const Bitset& a, const Bitset& b);
			void PerformsXOR(const Bitset& a, const Bitset& b);

			bool Intersects(const Bitset& bitset) const;

			void Reserve(unsigned int bitCount);
			void Resize(unsigned int bitCount, bool defaultVal = false);

			void Reset();
			void Reset(unsigned int bit);

			void Set(bool val = true);
			void Set(unsigned int bit, bool val = true);
			void SetBlock(unsigned int i, Block block);

			void Swap(Bitset& bitset);

			bool Test(unsigned int bit) const;
			bool TestAll() const;
			bool TestAny() const;
			bool TestNone() const;

			template<typename T> T To() const;
			String ToString() const;

			void UnboundedReset(unsigned int bit);
			void UnboundedSet(unsigned int bit, bool val = true);
			bool UnboundedTest(unsigned int bit) const;

			Bit operator[](int index);
			bool operator[](int index) const;

			Bitset operator~() const;

			Bitset& operator=(const Bitset& bitset) = default;
			Bitset& operator=(const String& bits);
			template<typename T> Bitset& operator=(T value);
			Bitset& operator=(Bitset&& bitset) noexcept = default;

			Bitset& operator&=(const Bitset& bitset);
			Bitset& operator|=(const Bitset& bitset);
			Bitset& operator^=(const Bitset& bitset);

			static constexpr Block fullBitMask = std::numeric_limits<Block>::max();
			static constexpr unsigned int bitsPerBlock = std::numeric_limits<Block>::digits;
			static constexpr unsigned int npos = std::numeric_limits<unsigned int>::max();

		private:
			unsigned int FindFirstFrom(unsigned int blockIndex) const;
			Block GetLastBlockMask() const;
			void ResetExtraBits();

			static unsigned int ComputeBlockCount(unsigned int bitCount);
			static unsigned int GetBitIndex(unsigned int bit);
			static unsigned int GetBlockIndex(unsigned int bit);

			std::vector<Block, Allocator> m_blocks;
			unsigned int m_bitCount;
	};

	template<typename Block, class Allocator>
	class Bitset<Block, Allocator>::Bit
	{
		friend Bitset<Block, Allocator>;

		public:
			Bit(const Bit& bit) = default;

			Bit& Flip();
			Bit& Reset();
			Bit& Set(bool val = true);
			bool Test() const;

			template<bool BadCall = true>
			void* operator&() const;

			operator bool() const;
			Bit& operator=(bool val);
			Bit& operator=(const Bit& bit);

			Bit& operator|=(bool val);
			Bit& operator&=(bool val);
			Bit& operator^=(bool val);
			Bit& operator-=(bool val);

		private:
			Bit(Block& block, Block mask) :
			m_block(block),
			m_mask(mask)
			{
			}

			Block& m_block;
			Block m_mask;
	};

	template<typename Block, class Allocator>
	bool operator==(const Bitset<Block, Allocator>& lhs, const Nz::Bitset<Block, Allocator>& rhs);

	template<typename Block, class Allocator>
	bool operator!=(const Bitset<Block, Allocator>& lhs, const Nz::Bitset<Block, Allocator>& rhs);

	template<typename Block, class Allocator>
	bool operator<(const Bitset<Block, Allocator>& lhs, const Nz::Bitset<Block, Allocator>& rhs);

	template<typename Block, class Allocator>
	bool operator<=(const Bitset<Block, Allocator>& lhs, const Nz::Bitset<Block, Allocator>& rhs);

	template<typename Block, class Allocator>
	bool operator>(const Bitset<Block, Allocator>& lhs, const Nz::Bitset<Block, Allocator>& rhs);

	template<typename Block, class Allocator>
	bool operator>=(const Bitset<Block, Allocator>& lhs, const Nz::Bitset<Block, Allocator>& rhs);

	template<typename Block, class Allocator>
	Bitset<Block, Allocator> operator&(const Bitset<Block, Allocator>& lhs, const Bitset<Block, Allocator>& rhs);

	template<typename Block, class Allocator>
	Bitset<Block, Allocator> operator|(const Bitset<Block, Allocator>& lhs, const Bitset<Block, Allocator>& rhs);

	template<typename Block, class Allocator>
	Bitset<Block, Allocator> operator^(const Bitset<Block, Allocator>& lhs, const Bitset<Block, Allocator>& rhs);
}

namespace std
{
	template<typename Block, class Allocator>
	void swap(Nz::Bitset<Block, Allocator>& lhs, Nz::Bitset<Block, Allocator>& rhs);
}

#include <Nazara/Core/Bitset.inl>

#endif // NAZARA_BITSET_HPP

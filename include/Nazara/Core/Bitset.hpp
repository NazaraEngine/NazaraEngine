// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BITSET_HPP
#define NAZARA_BITSET_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <memory>
#include <type_traits>

class NzAbstractHash;

template<typename Block = nzUInt32, class Allocator = std::allocator<Block>>
class NzBitset
{
	static_assert(std::is_integral<Block>() && std::is_unsigned<Block>(), "Block must be a unsigned integral type");

	public:
		class Bit;

		NzBitset();
		explicit NzBitset(unsigned int bitCount, bool val = false);
		explicit NzBitset(const char* bits);
		NzBitset(const char* bits, unsigned int bitCount);
		NzBitset(const NzBitset& bitset) = default;
		explicit NzBitset(const NzString& bits);
		NzBitset(NzBitset&& bitset) noexcept = default;
		~NzBitset() = default;

		void Clear();
		unsigned int Count() const;
		void Flip();

		unsigned int FindFirst() const;
		unsigned int FindNext(unsigned int bit) const;

		Block GetBlock(unsigned int i) const;
		unsigned int GetBlockCount() const;
		unsigned int GetCapacity() const;
		unsigned int GetSize() const;

		void PerformsAND(const NzBitset& a, const NzBitset& b);
		void PerformsNOT(const NzBitset& a);
		void PerformsOR(const NzBitset& a, const NzBitset& b);
		void PerformsXOR(const NzBitset& a, const NzBitset& b);

		bool Intersects(const NzBitset& bitset) const;

		void Reserve(unsigned int bitCount);
		void Resize(unsigned int bitCount, bool defaultVal = false);

		void Reset();
		void Reset(unsigned int bit);

		void Set(bool val = true);
		void Set(unsigned int bit, bool val = true);
		void SetBlock(unsigned int i, Block block);

		void Swap(NzBitset& bitset);

		bool Test(unsigned int bit) const;
		bool TestAll();
		bool TestAny();
		bool TestNone();

		template<typename T> T To() const;
		NzString ToString() const;

		void UnboundedReset(unsigned int bit);
		void UnboundedSet(unsigned int bit, bool val = true);
		bool UnboundedTest(unsigned int bit) const;

		Bit operator[](int index);
		bool operator[](int index) const;

		NzBitset operator~() const;

		NzBitset& operator=(const NzBitset& bitset) = default;
		NzBitset& operator=(const NzString& bits);
		NzBitset& operator=(NzBitset&& bitset) noexcept = default;

		NzBitset& operator&=(const NzBitset& bitset);
		NzBitset& operator|=(const NzBitset& bitset);
		NzBitset& operator^=(const NzBitset& bitset);

		static Block fullBitMask;
		static unsigned int bitsPerBlock;
		static unsigned int npos;

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

template<typename Block, typename Allocator>
bool operator==(const NzBitset<Block, Allocator>& lhs, const NzBitset<Block, Allocator>& rhs);

template<typename Block, typename Allocator>
bool operator!=(const NzBitset<Block, Allocator>& lhs, const NzBitset<Block, Allocator>& rhs);

template<typename Block, typename Allocator>
NzBitset<Block, Allocator> operator&(const NzBitset<Block, Allocator>& lhs, const NzBitset<Block, Allocator>& rhs);

template<typename Block, typename Allocator>
NzBitset<Block, Allocator> operator|(const NzBitset<Block, Allocator>& lhs, const NzBitset<Block, Allocator>& rhs);

template<typename Block, typename Allocator>
NzBitset<Block, Allocator> operator^(const NzBitset<Block, Allocator>& lhs, const NzBitset<Block, Allocator>& rhs);

template<typename Block, typename Allocator>
class NzBitset<Block, Allocator>::Bit
{
	friend NzBitset<Block, Allocator>;

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

namespace std
{
	template<typename Block, class Allocator>
	void swap(NzBitset<Block, Allocator>& lhs, NzBitset<Block, Allocator>& rhs);
}

#include <Nazara/Core/Bitset.inl>

#endif // NAZARA_BITSET_HPP

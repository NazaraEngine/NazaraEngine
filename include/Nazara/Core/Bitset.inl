// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <limits>
#include <utility>
#include <Nazara/Core/Debug.hpp>

template<typename Block, class Allocator>
NzBitset<Block, Allocator>::NzBitset() :
m_bitCount(0)
{
}

template<typename Block, class Allocator>
NzBitset<Block, Allocator>::NzBitset(unsigned int bitCount, bool val) :
NzBitset()
{
	Resize(bitCount, val);
}

template<typename Block, class Allocator>
NzBitset<Block, Allocator>::NzBitset(const char* bits) :
NzBitset(bits, std::strlen(bits))
{
}

template<typename Block, class Allocator>
NzBitset<Block, Allocator>::NzBitset(const char* bits, unsigned int bitCount) :
m_blocks(ComputeBlockCount(bitCount), 0U),
m_bitCount(bitCount)
{
	for (unsigned int i = 0; i < bitCount; ++i)
	{
		switch (*bits++)
		{
			case '1':
				// On adapte l'indice (inversion par rapport à la chaîne)
				Set(m_bitCount - i - 1, true);
				break;

			case '0':
				// Tous les blocs ont été initialisés à zéro, rien à faire ici
				break;

			default:
				NazaraAssert(false, "Unexpected char (neither 1 nor 0)");
				break;
		}
	}
}

template<typename Block, class Allocator>
NzBitset<Block, Allocator>::NzBitset(const NzString& bits) :
NzBitset(bits.GetConstBuffer(), bits.GetSize())
{
}

template<typename Block, class Allocator>
void NzBitset<Block, Allocator>::Clear()
{
	m_bitCount = 0;
	m_blocks.clear();
}

template<typename Block, class Allocator>
unsigned int NzBitset<Block, Allocator>::Count() const
{
	if (m_blocks.empty())
		return 0;

	unsigned int count = 0;
	for (unsigned int i = 0; i < m_blocks.size(); ++i)
		count += NzCountBits(m_blocks[i]);

	return count;
}

template<typename Block, class Allocator>
void NzBitset<Block, Allocator>::Flip()
{
	for (Block& block : m_blocks)
		block ^= fullBitMask;

	ResetExtraBits();
}

template<typename Block, class Allocator>
unsigned int NzBitset<Block, Allocator>::FindFirst() const
{
	return FindFirstFrom(0);
}

template<typename Block, class Allocator>
unsigned int NzBitset<Block, Allocator>::FindNext(unsigned int bit) const
{
	NazaraAssert(bit < m_bitCount, "Bit index out of range");

	bit++;

	// Le bloc du bit, l'indice du bit
	unsigned int blockIndex = GetBlockIndex(bit);
	unsigned int bitIndex = GetBitIndex(bit);

	// Récupération du bloc
	Block block = m_blocks[blockIndex];

	// On ignore les X premiers bits
	block >>= bitIndex;

	// Si le bloc n'est pas nul, c'est bon, sinon on doit chercher à partir du prochain bloc
	if (block)
		return NzIntegralLog2Pot(block & -block) + bit;
	else
		return FindFirstFrom(blockIndex + 1);
}

template<typename Block, class Allocator>
Block NzBitset<Block, Allocator>::GetBlock(unsigned int i) const
{
	NazaraAssert(i < m_blocks.size(), "Block index out of range");

	return m_blocks[i];
}

template<typename Block, class Allocator>
unsigned int NzBitset<Block, Allocator>::GetBlockCount() const
{
	return m_blocks.size();
}

template<typename Block, class Allocator>
unsigned int NzBitset<Block, Allocator>::GetCapacity() const
{
	return m_blocks.capacity()*bitsPerBlock;
}

template<typename Block, class Allocator>
unsigned int NzBitset<Block, Allocator>::GetSize() const
{
	return m_bitCount;
}

template<typename Block, class Allocator>
void NzBitset<Block, Allocator>::PerformsAND(const NzBitset& a, const NzBitset& b)
{
	std::pair<unsigned int, unsigned int> minmax = std::minmax(a.GetBlockCount(), b.GetBlockCount());

	m_blocks.resize(minmax.second);
	m_bitCount = std::max(a.GetSize(), b.GetSize());

	// Dans le cas du AND, nous pouvons nous arrêter à la plus petite taille (car x & 0 = 0)
	for (unsigned int i = 0; i < minmax.first; ++i)
		m_blocks[i] = a.GetBlock(i) & b.GetBlock(i);

	ResetExtraBits();
}

template<typename Block, class Allocator>
void NzBitset<Block, Allocator>::PerformsNOT(const NzBitset& a)
{
	m_blocks.resize(a.GetBlockCount());
	m_bitCount = a.GetSize();

	for (unsigned int i = 0; i < m_blocks.size(); ++i)
		m_blocks[i] = ~a.GetBlock(i);

	ResetExtraBits();
}

template<typename Block, class Allocator>
void NzBitset<Block, Allocator>::PerformsOR(const NzBitset& a, const NzBitset& b)
{
	const NzBitset& greater = (a.GetBlockCount() > b.GetBlockCount()) ? a : b;
	const NzBitset& lesser = (a.GetBlockCount() > b.GetBlockCount()) ? b : a;

	unsigned int maxBlockCount = greater.GetBlockCount();
	unsigned int minBlockCount = lesser.GetBlockCount();
	m_blocks.resize(maxBlockCount);
	m_bitCount = greater.GetSize();

	for (unsigned int i = 0; i < minBlockCount; ++i)
		m_blocks[i] = a.GetBlock(i) | b.GetBlock(i);

	for (unsigned int i = minBlockCount; i < maxBlockCount; ++i)
		m_blocks[i] = greater.GetBlock(i); // (x | 0 = x)

	ResetExtraBits();
}

template<typename Block, class Allocator>
void NzBitset<Block, Allocator>::PerformsXOR(const NzBitset& a, const NzBitset& b)
{
	const NzBitset& greater = (a.GetBlockCount() > b.GetBlockCount()) ? a : b;
	const NzBitset& lesser = (a.GetBlockCount() > b.GetBlockCount()) ? b : a;

	unsigned int maxBlockCount = greater.GetBlockCount();
	unsigned int minBlockCount = lesser.GetBlockCount();
	m_blocks.resize(maxBlockCount);
	m_bitCount = greater.GetSize();

	for (unsigned int i = 0; i < minBlockCount; ++i)
		m_blocks[i] = a.GetBlock(i) ^ b.GetBlock(i);

	for (unsigned int i = minBlockCount; i < maxBlockCount; ++i)
		m_blocks[i] = greater.GetBlock(i); // (x ^ 0 = x)

	ResetExtraBits();
}

template<typename Block, class Allocator>
bool NzBitset<Block, Allocator>::Intersects(const NzBitset& bitset) const
{
	// On ne testera que les blocs en commun
	unsigned int sharedBlocks = std::min(GetBlockCount(), bitset.GetBlockCount());
	for (unsigned int i = 0; i < sharedBlocks; ++i)
	{
		Block a = GetBlock(i);
		Block b = bitset.GetBlock(i);
		if (a & b)
			return true;
	}

	return false;
}

template<typename Block, class Allocator>
void NzBitset<Block, Allocator>::Reserve(unsigned int bitCount)
{
	m_blocks.reserve(ComputeBlockCount(bitCount));
}

template<typename Block, class Allocator>
void NzBitset<Block, Allocator>::Resize(unsigned int bitCount, bool defaultVal)
{
	// On commence par changer la taille du conteneur, avec la valeur correcte d'initialisation
	auto oldSize = m_blocks.size();
	m_blocks.resize(ComputeBlockCount(bitCount), (defaultVal) ? fullBitMask : 0U);

	unsigned int remainingBits = GetBitIndex(m_bitCount);
	if (bitCount > m_bitCount && remainingBits > 0 && defaultVal)
	{
		// Initialisation des bits non-utilisés du dernier bloc avant le changement de taille
		Block& block = m_blocks[oldSize-1]; // Le bloc à corriger
		Block mask = fullBitMask << remainingBits; // Le masque sur les bits en question

		// Set/Reset des bits
		/*
		if (defaultVal)
			block |= mask;
		else
			block &= ~mask;
		*/
		// https://graphics.stanford.edu/~seander/bithacks.html#ConditionalSetOrClearBitsWithoutBranching
		block = (block & ~mask) | (-defaultVal & mask);
	}

	m_bitCount = bitCount;
	ResetExtraBits();
}

template<typename Block, class Allocator>
void NzBitset<Block, Allocator>::Reset()
{
	Set(false);
}

template<typename Block, class Allocator>
void NzBitset<Block, Allocator>::Set(bool val)
{
	std::fill(m_blocks.begin(), m_blocks.end(), (val) ? fullBitMask : Block(0U));
	if (val)
		ResetExtraBits();
}

template<typename Block, class Allocator>
void NzBitset<Block, Allocator>::Set(unsigned int bit, bool val)
{
	NazaraAssert(bit < m_bitCount, "Bit index out of range");

	Block& block = m_blocks[GetBlockIndex(bit)];
	Block mask = Block(1U) << GetBitIndex(bit);

	// Activation du bit sans branching
	// https://graphics.stanford.edu/~seander/bithacks.html#ConditionalSetOrClearBitsWithoutBranching
	block = (block & ~mask) | (-val & mask);
}

template<typename Block, class Allocator>
void NzBitset<Block, Allocator>::SetBlock(unsigned int i, Block block)
{
	NazaraAssert(i < m_blocks.size(), "Block index out of range");

	m_blocks[i] = block;
	if (i == m_blocks.size()-1)
		ResetExtraBits();
}

template<typename Block, class Allocator>
void NzBitset<Block, Allocator>::Swap(NzBitset& bitset)
{
	std::swap(m_bitCount, bitset.m_bitCount);
	std::swap(m_blocks, bitset.m_blocks);
}

template<typename Block, class Allocator>
bool NzBitset<Block, Allocator>::Test(unsigned int bit) const
{
	NazaraAssert(bit < m_bitCount, "Bit index out of range");

	return m_blocks[GetBlockIndex(bit)] & (Block(1U) << GetBitIndex(bit));
}

template<typename Block, class Allocator>
bool NzBitset<Block, Allocator>::TestAll()
{
	// Cas particulier du dernier bloc
	Block lastBlockMask = GetLastBlockMask();

	for (unsigned int i = 0; i < m_blocks.size(); ++i)
	{
		Block mask = (i == m_blocks.size() - 1) ? lastBlockMask : fullBitMask;
		if (m_blocks[i] == mask) // Les extra bits sont à zéro, on peut donc tester sans procéder à un masquage
			return false;
	}

	return true;
}

template<typename Block, class Allocator>
bool NzBitset<Block, Allocator>::TestAny()
{
	if (m_blocks.empty())
		return false;

	for (unsigned int i = 0; i < m_blocks.size(); ++i)
	{
		if (m_blocks[i])
			return true;
	}

	return false;
}

template<typename Block, class Allocator>
bool NzBitset<Block, Allocator>::TestNone()
{
	return !TestAny();
}

template<typename Block, class Allocator>
template<typename T>
T NzBitset<Block, Allocator>::To() const
{
	static_assert(std::is_integral<T>() && std::is_unsigned<T>(), "T must be a unsigned integral type");

	NazaraAssert(m_bitCount <= std::numeric_limits<T>::digits, "Bit count cannot be greater than UInt32 bit count");

	T value = 0;
	for (unsigned int i = 0; i < m_blocks.size(); ++i)
		value |= static_cast<T>(m_blocks[i]) << i*bitsPerBlock;

	return value;
}

template<typename Block, class Allocator>
NzString NzBitset<Block, Allocator>::ToString() const
{
	NzString str(m_bitCount, '0');

	for (unsigned int i = 0; i < m_bitCount; ++i)
	{
		if (Test(i))
			str[m_bitCount - i - 1] = '1'; // Inversion de l'indice
	}

	return str;
}

template<typename Block, class Allocator>
typename NzBitset<Block, Allocator>::Bit NzBitset<Block, Allocator>::operator[](int index)
{
	return Bit(m_blocks[GetBlockIndex(index)], Block(1U) << GetBitIndex(index));
}

template<typename Block, class Allocator>
bool NzBitset<Block, Allocator>::operator[](int index) const
{
	return Test(index);
}

template<typename Block, class Allocator>
NzBitset<Block, Allocator> NzBitset<Block, Allocator>::operator~() const
{
	NzBitset bitset;
	bitset.PerformsNOT(*this);

	return bitset;
}

template<typename Block, class Allocator>
NzBitset<Block, Allocator>& NzBitset<Block, Allocator>::operator=(const NzString& bits)
{
	NzBitset bitset(bits);
	std::swap(*this, bitset);

	return *this;
}

template<typename Block, class Allocator>
NzBitset<Block, Allocator>& NzBitset<Block, Allocator>::operator&=(const NzBitset& bitset)
{
	PerformsAND(*this, bitset);

	return *this;
}

template<typename Block, class Allocator>
NzBitset<Block, Allocator>& NzBitset<Block, Allocator>::operator|=(const NzBitset& bitset)
{
	PerformsOR(*this, bitset);

	return *this;
}

template<typename Block, class Allocator>
NzBitset<Block, Allocator>& NzBitset<Block, Allocator>::operator^=(const NzBitset& bitset)
{
	PerformsXOR(*this, bitset);

	return *this;
}

template<typename Block, class Allocator>
unsigned int NzBitset<Block, Allocator>::FindFirstFrom(unsigned int blockIndex) const
{
	if (blockIndex >= m_blocks.size())
		return npos;

	// On cherche le premier bloc non-nul
	unsigned int i = blockIndex;
	for (; i < m_blocks.size(); ++i)
	{
		if (m_blocks[i])
			break;
	}
	Block block = m_blocks[i];

	// Calcul de la position du LSB dans le bloc (et ajustement de la position)
	return NzIntegralLog2Pot(block & -block) + i*bitsPerBlock;
}

template<typename Block, class Allocator>
Block NzBitset<Block, Allocator>::GetLastBlockMask() const
{
	return (Block(1U) << GetBitIndex(m_bitCount)) - 1U;
}

template<typename Block, class Allocator>
void NzBitset<Block, Allocator>::ResetExtraBits()
{
	Block mask = GetLastBlockMask();
	if (mask)
		m_blocks.back() &= mask;
}

template<typename Block, class Allocator>
unsigned int NzBitset<Block, Allocator>::ComputeBlockCount(unsigned int bitCount)
{
	return GetBlockIndex(bitCount) + ((GetBitIndex(bitCount) != 0U) ? 1U : 0U);
}

template<typename Block, class Allocator>
unsigned int NzBitset<Block, Allocator>::GetBitIndex(unsigned int bit)
{
	return bit & (bitsPerBlock - 1U); // bit % bitsPerBlock
}

template<typename Block, class Allocator>
unsigned int NzBitset<Block, Allocator>::GetBlockIndex(unsigned int bit)
{
	return bit / bitsPerBlock;
}

template<typename Block, class Allocator>
Block NzBitset<Block, Allocator>::fullBitMask = std::numeric_limits<Block>::max();

template<typename Block, class Allocator>
unsigned int NzBitset<Block, Allocator>::bitsPerBlock = std::numeric_limits<Block>::digits;

template<typename Block, class Allocator>
unsigned int NzBitset<Block, Allocator>::npos = std::numeric_limits<unsigned int>::max();


template<typename Block, typename Allocator>
bool operator==(const NzBitset<Block, Allocator>& lhs, const NzBitset<Block, Allocator>& rhs)
{
	// La comparaison part du principe que (uint8) 00001100 == (uint16) 00000000 00001100
	// et conserve donc cette propriété
	const NzBitset<Block, Allocator>& greater = (lhs.GetBlockCount() > rhs.GetBlockCount()) ? lhs : rhs;
	const NzBitset<Block, Allocator>& lesser = (lhs.GetBlockCount() > rhs.GetBlockCount()) ? rhs : lhs;

	unsigned int maxBlockCount = greater.GetBlockCount();
	unsigned int minBlockCount = lesser.GetBlockCount();

	// Nous testons les blocs en commun pour vérifier l'égalité des bits
	for (unsigned int i = 0; i < minBlockCount; ++i)
	{
		if (lhs.GetBlock(i) != rhs.GetBlock(i))
			return false;
	}

	// Nous vérifions maintenant les blocs que seul le plus grand bitset possède, pour prétendre à l'égalité
	// ils doivent tous être nuls
	for (unsigned int i = minBlockCount; i < maxBlockCount; ++i)
		if (greater.GetBlock(i))
			return false;

	return true;
}

template<typename Block, typename Allocator>
bool operator!=(const NzBitset<Block, Allocator>& lhs, const NzBitset<Block, Allocator>& rhs)
{
	return !(lhs == rhs);
}

template<typename Block, typename Allocator>
NzBitset<Block, Allocator> operator&(const NzBitset<Block, Allocator>& lhs, const NzBitset<Block, Allocator>& rhs)
{
	NzBitset<Block, Allocator> bitset;
	bitset.PerformsAND(lhs, rhs);

	return bitset;
}

template<typename Block, typename Allocator>
NzBitset<Block, Allocator> operator|(const NzBitset<Block, Allocator>& lhs, const NzBitset<Block, Allocator>& rhs)
{
	NzBitset<Block, Allocator> bitset;
	bitset.PerformsOR(lhs, rhs);

	return bitset;
}

template<typename Block, typename Allocator>
NzBitset<Block, Allocator> operator^(const NzBitset<Block, Allocator>& lhs, const NzBitset<Block, Allocator>& rhs)
{
	NzBitset<Block, Allocator> bitset;
	bitset.PerformsXOR(lhs, rhs);

	return bitset;
}


template<typename Block, typename Allocator>
typename NzBitset<Block, Allocator>::Bit& NzBitset<Block, Allocator>::Bit::Flip()
{
	m_block ^= m_mask;

	return *this;
}

template<typename Block, typename Allocator>
typename NzBitset<Block, Allocator>::Bit& NzBitset<Block, Allocator>::Bit::Reset()
{
	return Set(false);
}

template<typename Block, typename Allocator>
typename NzBitset<Block, Allocator>::Bit& NzBitset<Block, Allocator>::Bit::Set(bool val)
{
	// https://graphics.stanford.edu/~seander/bithacks.html#ConditionalSetOrClearBitsWithoutBranching
	m_block = (m_block & ~m_mask) | (-val & m_mask);

	return *this;
}

template<typename Block, typename Allocator>
bool NzBitset<Block, Allocator>::Bit::Test() const
{
	return m_block & m_mask;
}

template<typename Block, typename Allocator>
template<bool BadCall>
void* NzBitset<Block, Allocator>::Bit::operator&() const
{
	// Le template est nécessaire pour ne planter la compilation qu'à l'utilisation
	static_assert(!BadCall, "It is impossible to take the address of a bit in a bitset");

	return nullptr;
}

template<typename Block, typename Allocator>
NzBitset<Block, Allocator>::Bit::operator bool() const
{
	return Test();
}

template<typename Block, typename Allocator>
typename NzBitset<Block, Allocator>::Bit& NzBitset<Block, Allocator>::Bit::operator=(bool val)
{
	return Set(val);
}

template<typename Block, typename Allocator>
typename NzBitset<Block, Allocator>::Bit& NzBitset<Block, Allocator>::Bit::operator=(const Bit& bit)
{
	return Set(bit);
}

template<typename Block, typename Allocator>
typename NzBitset<Block, Allocator>::Bit& NzBitset<Block, Allocator>::Bit::operator|=(bool val)
{
	// Version sans branching:
	Set((val) ? true : Test());

	// Avec branching:
	/*
	if (val)
		Set();
	*/

	return *this;
}

template<typename Block, typename Allocator>
typename NzBitset<Block, Allocator>::Bit& NzBitset<Block, Allocator>::Bit::operator&=(bool val)
{
	// Version sans branching:
	Set((val) ? Test() : false);

	// Avec branching:
	/*
	if (!val)
		Reset();
	*/

	return *this;
}

template<typename Block, typename Allocator>
typename NzBitset<Block, Allocator>::Bit& NzBitset<Block, Allocator>::Bit::operator^=(bool val)
{
	// Version sans branching:
	Set((val) ? !Test() : Test());

	// Avec branching:
	/*
	if (val)
		Flip();
	*/

	return *this;
}

template<typename Block, typename Allocator>
typename NzBitset<Block, Allocator>::Bit& NzBitset<Block, Allocator>::Bit::operator-=(bool val)
{
	// Version sans branching:
	Set((val) ? false : Test());

	// Avec branching:
	/*
	if (val)
		Reset();
	*/

	return *this;
}

namespace std
{
	template<typename Block, class Allocator>
	void swap(NzBitset<Block, Allocator>& lhs, NzBitset<Block, Allocator>& rhs)
	{
		lhs.Swap(rhs);
	}
}

#include <Nazara/Core/DebugOff.hpp>

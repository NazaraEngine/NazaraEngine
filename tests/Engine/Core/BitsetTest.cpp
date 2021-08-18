#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <catch2/catch.hpp>
#include <array>
#include <string>
#include <iostream>

template<typename Block> void Check(const char* title);
template<typename Block> void CheckAppend(const char* title);
template<typename Block> void CheckBitOps(const char* title);
template<typename Block> void CheckBitOpsMultipleBlocks(const char* title);
template<typename Block> void CheckConstructor(const char* title);
template<typename Block> void CheckCopyMoveSwap(const char* title);
template<typename Block> void CheckRead(const char* title);
template<typename Block> void CheckReverse(const char* title);

SCENARIO("Bitset", "[CORE][BITSET]")
{
	Check<Nz::UInt8>("Bitset made of 8bits blocks");
	Check<Nz::UInt16>("Bitset made of 16bits blocks");
	Check<Nz::UInt32>("Bitset made of 32bits blocks");
	Check<Nz::UInt64>("Bitset made of 64bits blocks");
}

template<typename Block>
void Check(const char* title)
{
	CheckConstructor<Block>(title);
	CheckCopyMoveSwap<Block>(title);

	CheckBitOps<Block>(title);
	CheckBitOpsMultipleBlocks<Block>(title);

	CheckAppend<Block>(title);
	CheckRead<Block>(title);
	CheckReverse<Block>(title);
}

template<typename Block>
void CheckAppend(const char* title)
{
	SECTION(title)
	{
		GIVEN("An empty bitset filled by bytes")
		{
			#define BitVal1 00110111
			#define BitVal2 11011110
			#define BitVal3 01000010
			std::array<Nz::UInt8, 3> data = {{NazaraPrefixMacro(BitVal1, 0b), NazaraPrefixMacro(BitVal2, 0b), NazaraPrefixMacro(BitVal3, 0b)}};
			const char result[] = NazaraStringifyMacro(BitVal3) NazaraStringifyMacro(BitVal2) NazaraStringifyMacro(BitVal1);
			std::size_t bitCount = data.size() * 8;
			#undef BitVal1
			#undef BitVal2
			#undef BitVal3

			std::array<std::pair<const char*, std::size_t>, 7> tests = {
				{
					{"We append bits one by one", 1},
					{"We append bits two by two", 2},
					{"We append bits three by three", 3},
					{"We append bits four by four", 4},
					{"We append bits six by six", 6},
					{"We append bits byte by byte", 8},
					{"We append bits twelve by twelve", 12}
				}
			};

			for (auto& pair : tests)
			{
				WHEN(pair.first)
				{
					Nz::Bitset<Block> bitset;

					for (std::size_t i = 0; i < bitCount; i += pair.second)
					{
						Nz::UInt16 value = data[i / 8] >> (i % 8);
						if ((i % 8) + pair.second > 8 && i/8 != data.size()-1)
							value |= static_cast<Nz::UInt16>(data[i / 8 + 1]) << (8 - (i % 8));

						bitset.AppendBits(value, pair.second);
					}

					REQUIRE(bitset.GetSize() == bitCount);

					Nz::Bitset<Block> expectedBitset(result);

					CHECK(bitset == expectedBitset);
					CHECK(bitset.GetBlockCount() == (bitCount / bitset.bitsPerBlock + std::min<std::size_t>(1, bitCount % bitset.bitsPerBlock)));
				}
			}
		}
	}
}

template<typename Block>
void CheckBitOps(const char* title)
{
	SECTION(title)
	{
		GIVEN("Two bitsets")
		{
			Nz::Bitset<Block> first("01001");
			Nz::Bitset<Block> second("10111");

			WHEN("We perform operators")
			{
				Nz::Bitset<Block> andBitset = first & second;
				Nz::Bitset<Block> orBitset = first | second;
				Nz::Bitset<Block> xorBitset = first ^ second;

				THEN("They should operate as logical operators")
				{
					CHECK(andBitset == Nz::Bitset<Block>("00001"));
					CHECK(orBitset  == Nz::Bitset<Block>("11111"));
					CHECK(orBitset.TestAll());
					CHECK(xorBitset == Nz::Bitset<Block>("11110"));
					CHECK(!xorBitset.TestAll());
					CHECK((~orBitset).TestNone());
				}
			}

			WHEN("We perform bit shifts")
			{
				first.ShiftLeft(1);
				second.ShiftRight(2);

				THEN("We should obtain these")
				{
					CHECK(first == Nz::Bitset<Block>("10010"));
					CHECK(second == Nz::Bitset<Block>("101"));
				}
			}
		}
	}
}

template<typename Block>
void CheckBitOpsMultipleBlocks(const char* title)
{
	SECTION(title)
	{
		GIVEN("Two bitsets")
		{
			Nz::Bitset<Block> first("01001011010010101001010011010101001");
			Nz::Bitset<Block> second("10111111101101110110111101101110110");

			WHEN("We perform operators")
			{
				Nz::Bitset<Block> andBitset = first & second;
				Nz::Bitset<Block> orBitset = first | second;
				Nz::Bitset<Block> xorBitset = first ^ second;

				THEN("They should operate as logical operators")
				{
					CHECK(andBitset == Nz::Bitset<Block>("00001011000000100000010001000100000"));
					CHECK(orBitset  == Nz::Bitset<Block>("11111111111111111111111111111111111"));
					CHECK(orBitset.TestAll());
					CHECK(xorBitset == Nz::Bitset<Block>("11110100111111011111101110111011111"));
					CHECK(!xorBitset.TestAll());
					CHECK((~orBitset).TestNone());
				}
			}

			WHEN("We perform bit shifts")
			{
				first.ShiftLeft(16);
				second.ShiftRight(16);

				THEN("We should obtain these")
				{
					CHECK(first == Nz::Bitset<Block>("10010100110101010010000000000000000"));
					first.ShiftLeft(1);
					CHECK(first == Nz::Bitset<Block>("00101001101010100100000000000000000"));
					CHECK(second == Nz::Bitset<Block>("1011111110110111011"));
					second.ShiftRight(1);
					CHECK(second == Nz::Bitset<Block>("101111111011011101"));
				}
			}
		}
	}
}

template<typename Block>
void CheckConstructor(const char* title)
{
	SECTION(title)
	{
		GIVEN("Allocate and constructor")
		{
			Nz::Bitset<Block> bitset(3, false);

			THEN("Capacity is 3 and size is 3")
			{
				CHECK(bitset.GetSize() == 3);
				CHECK(bitset.GetCapacity() >= 3);
			}
		}

		GIVEN("Iterator and default constructor")
		{
			std::string anotherDataString("0101");
			Nz::Bitset<Block> defaultByte;
			Nz::Bitset<Block> anotherData(anotherDataString.c_str());

			WHEN("We assign 'anotherData'")
			{
				defaultByte = anotherDataString;
				CHECK(anotherData == defaultByte);
				CHECK(defaultByte.GetSize() == 4);
				CHECK(defaultByte.GetCapacity() >= 4);
				CHECK(anotherData.GetSize() == 4);
				CHECK(anotherData.GetCapacity() >= 4);
			}
		}
	}
}

template<typename Block>
void CheckCopyMoveSwap(const char* title)
{
	SECTION(title)
	{
		GIVEN("Copy and Move constructor")
		{
			Nz::Bitset<Block> originalArray(3, true);

			WHEN("We copy")
			{
				Nz::Bitset<Block> copyBitset(originalArray);

				THEN("We get a copy")
				{
					CHECK(copyBitset == originalArray);

					AND_WHEN("We modify one")
					{
						for (std::size_t i = 0; i < copyBitset.GetSize(); ++i)
							copyBitset[i] = false;

						THEN("They are no more equal")
						{
							CHECK(copyBitset != originalArray);
							CHECK(copyBitset == Nz::Bitset<Block>(3, false));
						}
					}
				}
			}

			WHEN("We move")
			{
				Nz::Bitset<Block> moveBitset(std::move(originalArray));

				THEN("These results are expected")
				{
					CHECK(moveBitset == Nz::Bitset<Block>(3, true));
					CHECK(originalArray.GetCapacity() == 0);
				}
			}
		}

		GIVEN("Three bitsets")
		{
			Nz::Bitset<Block> first("01001");
			Nz::Bitset<Block> second("10110");
			Nz::Bitset<Block> third;

			WHEN("We swap first and third, then second and third and finally third and first")
			{
				Nz::Bitset<Block> oldFirst(first);
				Nz::Bitset<Block> oldSecond(second);

				first.Swap(third);
				std::swap(second, third);
				third.Swap(first);

				THEN("First and second have been swapped and third is still empty.")
				{
					CHECK(oldFirst == second);
					CHECK(oldSecond == first);
					CHECK(third.GetSize() == 0);
				}
			}
		}
	}
}

template<typename Block>
void CheckRead(const char* title)
{
	SECTION(title)
	{
		GIVEN("An empty bitset filled by reading")
		{
			#define BitVal1 10010101
			#define BitVal2 11010010
			#define BitVal3 01101010
			std::array<Nz::UInt8, 3> data = {{NazaraPrefixMacro(BitVal1, 0b), NazaraPrefixMacro(BitVal2, 0b), NazaraPrefixMacro(BitVal3, 0b)}};
			const char result[] = NazaraStringifyMacro(BitVal3) NazaraStringifyMacro(BitVal2) NazaraStringifyMacro(BitVal1);
			std::size_t resultLength = Nz::CountOf(result) - 1;
			std::size_t bitCount = data.size() * 8;
			#undef BitVal1
			#undef BitVal2
			#undef BitVal3

			std::array<std::pair<const char*, std::size_t>, 8> tests = {
				{
					{"We read bits one by one", 1},
					{"We read bits two by two", 2},
					{"We read bits three by three", 3},
					{"We read bits four by four", 4},
					{"We read bits six by six", 6},
					{"We read bits byte by byte", 8},
					{"We read bits twelve by twelve", 12},
					{"We read bits all at once", 24}
				}
			};

			for (auto& pair : tests)
			{
				WHEN(pair.first)
				{
					Nz::Bitset<Block> bitset;

					auto seq = bitset.Write(data.data(), pair.second);
					for (std::size_t i = pair.second; i < bitCount; i += pair.second)
						seq = bitset.Write(seq, pair.second);

					REQUIRE(bitset.GetSize() == bitCount);

					Nz::Bitset<Block> expectedBitset(result);

					CHECK(bitset == expectedBitset);
					CHECK(bitset.GetBlockCount() == (bitCount / bitset.bitsPerBlock + std::min<std::size_t>(1, bitCount % bitset.bitsPerBlock)));
				}
			}
		}
	}
}

template<typename Block>
void CheckReverse(const char* title)
{
	SECTION(title)
	{
		GIVEN("A bitset")
		{
			std::string bits = "010011100010001101001111";
			Nz::Bitset<Block> expected(bits);

			WHEN("We reverse the order of bits")
			{
				Nz::Bitset<Block> bitset(bits);
				bitset.Reverse();

				THEN("The order of bits should be reversed")
				{
					std::string reversedBits = bits;
					std::reverse(reversedBits.begin(), reversedBits.end());
					CHECK(bitset == Nz::Bitset<Block>(reversedBits));
				}
				AND_WHEN("We reverse the bit order again")
				{
					bitset.Reverse();

					THEN("It should be back to normal")
					{
						CHECK(bitset == expected);
					}
				}
			}
		}
	}
}

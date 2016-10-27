#include <Nazara/Core/Bitset.hpp>
#include <Catch/catch.hpp>

#include <string>

SCENARIO("Bitset", "[CORE][BITSET]")
{
	GIVEN("Allocate and constructor")
	{
		Nz::Bitset<> bitset(3, false);

		THEN("Capacity is 3 and size is 3")
		{
			CHECK(bitset.GetSize() == 3);
			CHECK(bitset.GetCapacity() >= 3);
		}
	}

	GIVEN("Iterator and default constructor")
	{
		Nz::String anotherDataString("0101");
		Nz::Bitset<> defaultByte;
		Nz::Bitset<> anotherData(anotherDataString.GetConstBuffer());

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

	GIVEN("Copy and Move constructor")
	{
		Nz::Bitset<> originalArray(3, true);

		WHEN("We copy")
		{
			Nz::Bitset<> copyBitset(originalArray);

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
						CHECK(copyBitset == Nz::Bitset<>(3, false));
					}
				}
			}
		}

		WHEN("We move")
		{
			Nz::Bitset<> moveBitset(std::move(originalArray));

			THEN("These results are expected")
			{
				REQUIRE(moveBitset == Nz::Bitset<>(3, true));
				REQUIRE(originalArray.GetCapacity() == 0);
			}
		}
	}

	GIVEN("Three bitsets")
	{
		Nz::Bitset<> first("01001");
		Nz::Bitset<> second("10110");
		Nz::Bitset<> third;

		WHEN("We swap first and third, then second and third and finally third and first")
		{
			Nz::Bitset<> oldFirst(first);
			Nz::Bitset<> oldSecond(second);

			first.Swap(third);
			std::swap(second, third);
			third.Swap(first);

			THEN("First and second have been swapped and third is still empty.")
			{
				REQUIRE(oldFirst == second);
				REQUIRE(oldSecond == first);
				REQUIRE(third.GetSize() == 0);
			}
		}
	}

	GIVEN("Two bitsets")
	{
		Nz::Bitset<> first("01001");
		Nz::Bitset<> second("10111");

		WHEN("We perform operators")
		{
			Nz::Bitset<> andBitset = first & second;
			Nz::Bitset<> orBitset = first | second;
			Nz::Bitset<> xorBitset = first ^ second;

			THEN("They should operate as logical operators")
			{
				REQUIRE(andBitset == Nz::Bitset<>("00001"));
				REQUIRE(orBitset == Nz::Bitset<>("11111"));
				REQUIRE(xorBitset == Nz::Bitset<>("11110"));
			}
		}
	}
}

#include <Nazara/Core/ByteArray.hpp>
#include <Catch/catch.hpp>

#include <string>

SCENARIO("ByteArray", "[CORE][BYTEARRAY]")
{
	GIVEN("Allocate and raw constructor")
	{
		NzByteArray byteArray(3);

		THEN("Capacity is 3 and size is 0")
		{
			REQUIRE(byteArray.GetSize() == 0);
			REQUIRE(byteArray.GetCapacity() >= 3);
		}

		WHEN("We add 'data'")
		{
			byteArray.Append("data", 4);

			THEN("We get 'data'")
			{
				REQUIRE(byteArray.GetSize() == 4);
				REQUIRE(byteArray.GetCapacity() >= 4);
				REQUIRE(byteArray == NzByteArray("data", 4));
				REQUIRE(byteArray.ToString() == "data");
			}
		}
	}

	GIVEN("Iterator and default constructor")
	{
		std::string anotherDataString("anotherData");
		NzByteArray defaultByte;
		NzByteArray anotherData(anotherDataString.begin(), anotherDataString.end());

		WHEN("We assign 'anotherData' with iterator")
		{
			defaultByte.Assign(anotherData.begin(), anotherData.end());
			REQUIRE(anotherData == defaultByte);
			REQUIRE(defaultByte.GetSize() == 11);
			REQUIRE(defaultByte.GetCapacity() >= 11);
			REQUIRE(anotherData.GetSize() == 11);
			REQUIRE(anotherData.GetCapacity() >= 11);
		}
	}

	GIVEN("Copy and Move constructor")
	{
		NzByteArray originalArray(3, 64);

		WHEN("We copy")
		{
			NzByteArray copyByteArray(originalArray);

			THEN("We get a copy")
			{
				REQUIRE(copyByteArray == originalArray);

				AND_WHEN("We modify one")
				{
					for (NzByteArray::size_type i = 0; i < copyByteArray.GetSize(); ++i)
						copyByteArray[i] = 46;

					THEN("They are no more equal")
					{
						REQUIRE(copyByteArray != originalArray);
						REQUIRE(copyByteArray == NzByteArray(3, 46));
						REQUIRE(copyByteArray.GetConstBuffer() != originalArray.GetConstBuffer());
					}
				}
			}
		}

		WHEN("We move")
		{
			NzByteArray moveByteArray(std::move(originalArray));

			THEN("These results are expected")
			{
				REQUIRE(moveByteArray == NzByteArray(3, 64));
				CHECK(originalArray.IsEmpty());
				REQUIRE(originalArray.GetCapacity() == 0);
				REQUIRE(moveByteArray.GetConstBuffer() != originalArray.GetConstBuffer());

				AND_WHEN("We modify the empty one")
				{
					originalArray.Prepend(NzByteArray(3, 64));

					THEN("They are no more equal")
					{
						REQUIRE(moveByteArray == originalArray);
						REQUIRE(moveByteArray.GetConstBuffer() != originalArray.GetConstBuffer());
					}
				}
			}
		}
	}

	GIVEN("Two byte array (abc) and (cba)")
	{
		NzByteArray abc("abc", 3);
		NzByteArray cba;
		cba = std::move(NzByteArray("cba", 3));

		WHEN("We do some antagonists operations")
		{
			THEN("These results are expected")
			{
				REQUIRE(abc.Back() == cba.Front());

				abc.Erase(abc.begin(), abc.begin() + 1);
				abc.Erase(abc.begin());

				cba.Erase(cba.end() - 1, cba.end());
				cba.Erase(cba.end() - 1);

				REQUIRE(abc == NzByteArray("c", 1));
				REQUIRE(cba == NzByteArray("c", 1));

				std::string ab("ab");
				abc.Insert(abc.begin(), ab.begin(), ab.end());
				cba += NzByteArray("ba", 2);

				REQUIRE(abc == NzByteArray("abc", 3));
				REQUIRE(cba == NzByteArray("cba", 3));

				abc.PopBack();
				cba.PopFront();

				REQUIRE(abc == NzByteArray("ab", 2));
				REQUIRE(cba == NzByteArray("ba", 2));

				abc.PushBack('c');
				cba.PushFront('c');

				REQUIRE(abc == NzByteArray("abc", 3));
				REQUIRE(cba == NzByteArray("cba", 3));
			}
		}
	}

	GIVEN("One byte array of capacity 10")
	{
		NzByteArray capacityArray(10);

		WHEN("We reserve for 100")
		{
			capacityArray.Reserve(100);

			THEN("Capacity is 100")
			{
				REQUIRE(capacityArray.GetCapacity() == 100);
			}

			AND_WHEN("We add information and then shrink to fit")
			{
				capacityArray.Prepend("information", 11);
				capacityArray.ShrinkToFit();

				THEN("Capacity is 11")
				{
					REQUIRE(capacityArray.GetCapacity() == 11);
					REQUIRE(capacityArray.GetSize() == 11);
				}
			}
		}

		NzByteArray::const_pointer oldBuffer = capacityArray.GetConstBuffer();
		WHEN("We reserve for 5, add 'data' for 4 and then shrink to fit")
		{
			capacityArray.Reserve(5);
			THEN("Capacity is still 10")
			{
				REQUIRE(capacityArray.GetCapacity() == 10);
				REQUIRE(capacityArray.GetSize() == 0);
				REQUIRE(capacityArray.GetConstBuffer() == oldBuffer);
			}

			capacityArray.Append("data", 4);
			capacityArray.ShrinkToFit();

			THEN("Capacity is 4")
			{
				REQUIRE(capacityArray.GetConstBuffer() != oldBuffer);
				REQUIRE(capacityArray.GetCapacity() == 4);
				REQUIRE(capacityArray.GetSize() == 4);
			}
		}
	}

	GIVEN("Three byte array")
	{
		NzByteArray first("hello", 5);
		NzByteArray second("world", 5);
		NzByteArray third;

		WHEN("We swap first and third, then second and third and finally third and first")
		{
			NzByteArray oldFirst(first);
			NzByteArray oldSecond(second);

			first.Swap(third);
			std::swap(second, third);
			third.Swap(first);

			THEN("First and second have been swapped and third is still empty.")
			{
				REQUIRE(oldFirst == second);
				REQUIRE(oldSecond == first);
				REQUIRE(third.IsEmpty());
			}
		}
	}
}

#include <Nazara/Core/ByteArray.hpp>
#include <Catch/catch.hpp>

#include <string>

SCENARIO("ByteArray", "[CORE][BYTEARRAY]")
{
	GIVEN("Allocate and raw constructor")
	{
		Nz::ByteArray byteArray(3);

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
				REQUIRE(byteArray == Nz::ByteArray("data", 4));
				REQUIRE(byteArray.ToString() == "data");
			}
		}
	}

	GIVEN("Iterator and default constructor")
	{
		std::string anotherDataString("anotherData");
		Nz::ByteArray defaultByte;
		Nz::ByteArray anotherData(anotherDataString.begin(), anotherDataString.end());

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
		Nz::ByteArray originalArray(3, 64);

		WHEN("We copy")
		{
			Nz::ByteArray copyByteArray(originalArray);

			THEN("We get a copy")
			{
				REQUIRE(copyByteArray == originalArray);

				AND_WHEN("We modify one")
				{
					for (Nz::ByteArray::size_type i = 0; i < copyByteArray.GetSize(); ++i)
						copyByteArray[i] = 46;

					THEN("They are no more equal")
					{
						REQUIRE(copyByteArray != originalArray);
						REQUIRE(copyByteArray == Nz::ByteArray(3, 46));
						REQUIRE(copyByteArray.GetConstBuffer() != originalArray.GetConstBuffer());
					}
				}
			}
		}

		WHEN("We move")
		{
			Nz::ByteArray moveByteArray(std::move(originalArray));

			THEN("These results are expected")
			{
				REQUIRE(moveByteArray == Nz::ByteArray(3, 64));
				CHECK(originalArray.IsEmpty());
				REQUIRE(originalArray.GetCapacity() == 0);
				REQUIRE(moveByteArray.GetConstBuffer() != originalArray.GetConstBuffer());

				AND_WHEN("We modify the empty one")
				{
					originalArray.Prepend(Nz::ByteArray(3, 64));

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
		Nz::ByteArray abc("abc", 3);
		Nz::ByteArray cba;
		cba = Nz::ByteArray("cba", 3);

		WHEN("We do some antagonists operations")
		{
			THEN("These results are expected")
			{
				REQUIRE(abc.Back() == cba.Front());

				abc.Erase(abc.begin(), abc.begin() + 1);
				abc.Erase(abc.begin());

				cba.Erase(cba.end() - 1, cba.end());
				cba.Erase(cba.end() - 1);

				REQUIRE(abc == Nz::ByteArray("c", 1));
				REQUIRE(cba == Nz::ByteArray("c", 1));

				std::string ab("ab");
				abc.Insert(abc.begin(), ab.begin(), ab.end());
				cba += Nz::ByteArray("ba", 2);

				REQUIRE(abc == Nz::ByteArray("abc", 3));
				REQUIRE(cba == Nz::ByteArray("cba", 3));

				abc.PopBack();
				cba.PopFront();

				REQUIRE(abc == Nz::ByteArray("ab", 2));
				REQUIRE(cba == Nz::ByteArray("ba", 2));

				abc.PushBack('c');
				cba.PushFront('c');

				REQUIRE(abc == Nz::ByteArray("abc", 3));
				REQUIRE(cba == Nz::ByteArray("cba", 3));
			}
		}
	}

	GIVEN("One byte array of capacity 10")
	{
		Nz::ByteArray capacityArray(10);

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

		Nz::ByteArray::const_pointer oldBuffer = capacityArray.GetConstBuffer();
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
		Nz::ByteArray first("hello", 5);
		Nz::ByteArray second("world", 5);
		Nz::ByteArray third;

		WHEN("We swap first and third, then second and third and finally third and first")
		{
			Nz::ByteArray oldFirst(first);
			Nz::ByteArray oldSecond(second);

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

	GIVEN("A default byte array")
	{
		Nz::ByteArray defaultByteArray;

		WHEN("We resize")
		{
			Nz::UInt64 oldSize = defaultByteArray.GetSize();
			REQUIRE(oldSize == 0);
			defaultByteArray.Resize(10);

			THEN("Capacity has increased")
			{
				REQUIRE(defaultByteArray[oldSize] == 0);

				REQUIRE(defaultByteArray.GetCapacity() >= 10);
				REQUIRE(defaultByteArray.GetSize() == 10);
			}
		}
	}
}

#include <Nazara/Core/SparsePtr.hpp>
#include <Catch/catch.hpp>

#include <array>

SCENARIO("SparsePtr", "[CORE][SPARSEPTR]")
{
	GIVEN("A sparse pointer pointing to an array with a stride of 2")
	{
		std::array<int, 5> arrays = { {0, 1, 2, 3, 4} };
		Nz::SparsePtr<int> sparsePtr(arrays.data(), 2 * sizeof(int));

		WHEN("We use operators")
		{
			THEN("Operator[] with 2 should be 4")
			{
				CHECK(4 == sparsePtr[2]);
			}

			THEN("Operator++ and Operator-- should be opposite")
			{
				++sparsePtr;
				CHECK(2 == *sparsePtr);
				auto old = sparsePtr++;
				CHECK(2 == *old);
				CHECK(4 == *sparsePtr);

				--sparsePtr;
				CHECK(2 == *sparsePtr);
				auto oldMinus = sparsePtr--;
				CHECK(2 == *oldMinus);
				CHECK(0 == *sparsePtr);
			}

			THEN("Operator+ and operator-")
			{
				auto offsetTwo = sparsePtr + 2;
				CHECK(4 == *offsetTwo);

				auto offsetZero = offsetTwo - 2;
				CHECK(0 == *offsetZero);

				CHECK((offsetTwo - offsetZero) == 2);
			}
		}
	}
}

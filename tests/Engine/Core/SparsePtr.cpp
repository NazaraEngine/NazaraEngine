#include <Nazara/Core/SparsePtr.hpp>
#include <Catch/catch.hpp>

#include <array>

SCENARIO("SparsePtr", "[CORE][SPARSEPTR]")
{
	GIVEN("A sparse pointer pointing to an array with a stride of 2")
	{
		std::array<int, 5> arrays{0, 1, 2, 3, 4};
		Nz::SparsePtr<int> sparsePtr(arrays.data(), 2 * sizeof(int));

		WHEN("We use operators")
		{
			THEN("Operator[] with 2 should be 4")
			{
				REQUIRE(4 == sparsePtr[2]);
			}

			THEN("Operator++ and Operator-- should be opposite")
			{
				++sparsePtr;
				REQUIRE(2 == *sparsePtr);
				auto old = sparsePtr++;
				REQUIRE(2 == *old);
				REQUIRE(4 == *sparsePtr);

				--sparsePtr;
				REQUIRE(2 == *sparsePtr);
				auto oldMinus = sparsePtr--;
				REQUIRE(2 == *oldMinus);
				REQUIRE(0 == *sparsePtr);
			}

			THEN("Operator+ and operator-")
			{
				auto offsetTwo = sparsePtr + 2;
				REQUIRE(4 == *offsetTwo);

				auto offsetZero = offsetTwo - 2;
				REQUIRE(0 == *offsetZero);

				REQUIRE((offsetTwo - offsetZero) == 2);
			}
		}
	}
}

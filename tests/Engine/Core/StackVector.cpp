#include <Nazara/Core/StackVector.hpp>
#include <Catch/catch.hpp>
#include <array>
#include <numeric>

SCENARIO("StackVector", "[CORE][STACKVECTOR]")
{
	GIVEN("A StackVector to contain multiple int")
	{
		volatile std::size_t capacity = 50;
		Nz::StackVector<int> vector = NazaraStackVector(int, capacity);

		WHEN("At construction, the vector is empty but has capacity")
		{
			CHECK(vector.capacity() == capacity);
			CHECK(vector.empty());
			CHECK(vector.size() == 0);
		}

		WHEN("Emplacing five elements, vector size increase accordingly")
		{
			for (std::size_t i = 0; i < 5; ++i)
			{
				int val = int(i);
				CHECK(vector.emplace_back(val) == val);
			}

			CHECK(!vector.empty());
			CHECK(vector.size() == 5);

			std::array<int, 5> expectedValues = { 0, 1, 2, 3, 4 };
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
		}

		WHEN("Pushing three elements, vector size increase accordingly")
		{
			for (std::size_t i = 0; i < 3; ++i)
			{
				int val = int(i);
				CHECK(vector.push_back(val) == val);
			}

			CHECK(!vector.empty());
			CHECK(vector.size() == 3);

			std::array<int, 3> expectedValues = { 0, 1, 2 };
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));

			THEN("We resize to five")
			{
				vector.resize(5);

				CHECK(!vector.empty());
				CHECK(vector.size() == 5);

				std::array<int, 5> expectedValues = { 0, 1, 2, 0, 0 };
				CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));

				AND_THEN("We resize it back to zero")
				{
					vector.resize(0);

					CHECK(vector.empty());
					CHECK(vector.size() == 0);
				}
				AND_THEN("We clear it")
				{
					vector.clear();

					CHECK(vector.empty());
					CHECK(vector.size() == 0);
				}
			}
		}

		WHEN("We generate its content will iota")
		{
			vector.resize(10);
			std::iota(vector.begin(), vector.end(), -5);

			std::array<int, 10> expectedValues = { -5, -4, -3, -2, -1, 0, 1, 2, 3, 4 };
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
		}

		WHEN("We generate its content using emplace")
		{
			for (std::size_t i = 0; i < 5; ++i)
			{
				int val = int(i);
				CHECK(*vector.emplace(vector.end(), val) == val);
			}

			CHECK(!vector.empty());
			CHECK(vector.size() == 5);

			std::array<int, 5> expectedValues = { 0, 1, 2, 3, 4 };
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
		}

		WHEN("We generate its content using emplace, in reverse order")
		{
			for (std::size_t i = 0; i < 5; ++i)
			{
				int val = int(i);
				CHECK(*vector.emplace(vector.begin(), val) == val);
			}

			CHECK(!vector.empty());
			CHECK(vector.size() == 5);

			std::array<int, 5> expectedValues = { 4, 3, 2, 1, 0 };
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
		}

		WHEN("We generate its content using emplace, at the middle")
		{
			for (std::size_t i = 0; i < 10; ++i)
			{
				int val = int(i);
				CHECK(*vector.emplace(vector.begin() + i / 2, val) == val);
			}

			CHECK(!vector.empty());
			CHECK(vector.size() == 10);

			std::array<int, 10> expectedValues = { 1, 3, 5, 7, 9, 8, 6, 4, 2, 0 };
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
		}

		WHEN("We generate its content using insert")
		{
			for (std::size_t i = 0; i < 5; ++i)
			{
				int val = int(i);
				CHECK(*vector.insert(vector.end(), val) == val);
			}

			CHECK(!vector.empty());
			CHECK(vector.size() == 5);

			std::array<int, 5> expectedValues = { 0, 1, 2, 3, 4 };
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
		}

		WHEN("We generate its content using insert, in reverse order")
		{
			for (std::size_t i = 0; i < 5; ++i)
			{
				int val = int(i);
				CHECK(*vector.insert(vector.begin(), val) == val);
			}

			CHECK(!vector.empty());
			CHECK(vector.size() == 5);

			std::array<int, 5> expectedValues = { 4, 3, 2, 1, 0 };
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
		}

		WHEN("We generate its content using insert, at the middle")
		{
			for (std::size_t i = 0; i < 10; ++i)
			{
				int val = int(i);
				CHECK(*vector.insert(vector.begin() + i / 2, val) == val);
			}

			CHECK(!vector.empty());
			CHECK(vector.size() == 10);

			std::array<int, 10> expectedValues = { 1, 3, 5, 7, 9, 8, 6, 4, 2, 0 };
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
		}
	}
}

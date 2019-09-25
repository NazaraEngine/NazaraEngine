#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Catch/catch.hpp>
#include <array>
#include <numeric>

// This is a quick way to check that checks are valid
#define USE_STD_VECTOR 0

class DestructionCounter
{
	public:
		DestructionCounter() :
		m_counter(nullptr),
		m_value(0)
		{
		}

		DestructionCounter(std::size_t* counter, int value) :
		m_counter(counter),
		m_value(value)
		{
			if (m_counter)
				(*m_counter)++;
		}

		DestructionCounter(const DestructionCounter& counter) :
		m_counter(counter.m_counter),
		m_value(counter.m_value)
		{
			if (m_counter)
				(*m_counter)++;
		}

		DestructionCounter(DestructionCounter&& counter) :
		m_counter(counter.m_counter),
		m_value(counter.m_value)
		{
			if (m_counter)
				(*m_counter)++;
		}

		~DestructionCounter()
		{
			if (m_counter)
			{
				assert(*m_counter > 0);
				(*m_counter)--;
			}
		}

		operator int() const
		{
			return m_value;
		}

		DestructionCounter& operator=(const DestructionCounter& counter)
		{
			if (m_counter)
			{
				assert(*m_counter > 0);
				(*m_counter)--;
			}

			m_counter = counter.m_counter;
			m_value = counter.m_value;

			if (m_counter)
				(*m_counter)++;

			return *this;
		}

		DestructionCounter& operator=(DestructionCounter&& counter)
		{
			if (this == &counter)
				return *this;

			if (m_counter)
			{
				assert(*m_counter > 0);
				(*m_counter)--;
			}

			m_counter = counter.m_counter;
			m_value = counter.m_value;

			if (m_counter)
				(*m_counter)++;

			return *this;
		}

	private:
		std::size_t* m_counter;
		int m_value;
};

SCENARIO("StackVector", "[CORE][STACKVECTOR]")
{
	GIVEN("A StackVector to contain multiple int")
	{
		volatile std::size_t capacity = 50;
#if USE_STD_VECTOR
		std::vector<DestructionCounter> vector;
		vector.reserve(capacity);
#else
		Nz::StackVector<DestructionCounter> vector = NazaraStackVector(DestructionCounter, capacity);
#endif
		std::size_t counter = 0;

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
#if USE_STD_VECTOR
				vector.emplace_back(&counter, int(i));
#else
				CHECK(vector.emplace_back(&counter, int(i)) == int(i));
#endif
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
				DestructionCounter val(&counter, int(i));
#if USE_STD_VECTOR
				vector.push_back(val);
#else
				CHECK(vector.push_back(val) == val);
#endif
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
			for (std::size_t i = 0; i < vector.size(); ++i)
				vector[i] = DestructionCounter(&counter, -5 + int(i));

			std::array<int, 10> expectedValues = { -5, -4, -3, -2, -1, 0, 1, 2, 3, 4 };
			CHECK(vector.size() == expectedValues.size());
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));

			AND_WHEN("We pop back some elements")
			{
				for (std::size_t i = 0; i < 5; ++i)
					vector.pop_back();

				std::array<int, 5> expectedValues = { -5, -4, -3, -2, -1 };
				CHECK(vector.size() == expectedValues.size());
				CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
			}
			AND_WHEN("We erase elements at the beginning")
			{
				vector.erase(vector.begin());
				vector.erase(vector.begin());

				std::array<int, 8> expectedValues = { -3, -2, -1, 0, 1, 2, 3, 4 };
				CHECK(vector.size() == expectedValues.size());
				CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
			}
			AND_WHEN("We erase elements in the middle")
			{
				vector.erase(vector.begin() + 2);
				vector.erase(vector.begin() + 2);
				vector.erase(vector.begin() + 6);

				std::array<int, 7> expectedValues = { -5, -4, -1, 0, 1, 2, 4 };
				CHECK(vector.size() == expectedValues.size());
				CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
			}
			AND_WHEN("We erase elements at the end")
			{
				vector.erase(vector.end() - 1);
				vector.erase(vector.end() - 1);

				std::array<int, 8> expectedValues = { -5, -4, -3, -2, -1, 0, 1, 2 };
				CHECK(vector.size() == expectedValues.size());
				CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
			}
			AND_WHEN("We erase a range")
			{
				vector.erase(vector.begin() + 2, vector.end() - 3);

				std::array<int, 5> expectedValues = { -5, -4, 2, 3, 4 };
				CHECK(vector.size() == expectedValues.size());
				CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
			}
			AND_WHEN("We erase everything")
			{
				vector.erase(vector.begin(), vector.end());

				CHECK(vector.empty());
			}
		}

		WHEN("We generate its content using emplace")
		{
			for (std::size_t i = 0; i < 5; ++i)
				CHECK(*vector.emplace(vector.end(), &counter, int(i)) == int(i));

			CHECK(!vector.empty());
			CHECK(vector.size() == 5);

			std::array<int, 5> expectedValues = { 0, 1, 2, 3, 4 };
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
		}

		WHEN("We generate its content using emplace, in reverse order")
		{
			for (std::size_t i = 0; i < 5; ++i)
				CHECK(*vector.emplace(vector.begin(), &counter, int(i)) == int(i));

			CHECK(!vector.empty());
			CHECK(vector.size() == 5);

			std::array<int, 5> expectedValues = { 4, 3, 2, 1, 0 };
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
		}

		WHEN("We generate its content using emplace, at the middle")
		{
			for (std::size_t i = 0; i < 10; ++i)
				CHECK(*vector.emplace(vector.begin() + i / 2, &counter, int(i)) == int(i));

			CHECK(!vector.empty());
			CHECK(vector.size() == 10);

			std::array<int, 10> expectedValues = { 1, 3, 5, 7, 9, 8, 6, 4, 2, 0 };
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
		}

		WHEN("We generate its content using insert")
		{
			for (std::size_t i = 0; i < 5; ++i)
				CHECK(*vector.insert(vector.end(), DestructionCounter(&counter, int(i))) == int(i));

			CHECK(!vector.empty());
			CHECK(vector.size() == 5);

			std::array<int, 5> expectedValues = { 0, 1, 2, 3, 4 };
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
		}

		WHEN("We generate its content using insert, in reverse order")
		{
			for (std::size_t i = 0; i < 5; ++i)
				CHECK(*vector.insert(vector.begin(), DestructionCounter(&counter, int(i))) == int(i));

			CHECK(!vector.empty());
			CHECK(vector.size() == 5);

			std::array<int, 5> expectedValues = { 4, 3, 2, 1, 0 };
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
		}

		WHEN("We generate its content using insert, at the middle")
		{
			for (std::size_t i = 0; i < 10; ++i)
				CHECK(*vector.insert(vector.begin() + i / 2, DestructionCounter(&counter, int(i))) == int(i));

			CHECK(!vector.empty());
			CHECK(vector.size() == 10);

			std::array<int, 10> expectedValues = { 1, 3, 5, 7, 9, 8, 6, 4, 2, 0 };
			CHECK(std::equal(vector.begin(), vector.end(), expectedValues.begin(), expectedValues.end()));
		}

		vector.clear();
		CHECK(counter == 0);
	}
}

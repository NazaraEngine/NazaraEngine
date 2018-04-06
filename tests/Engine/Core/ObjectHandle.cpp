#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <Catch/catch.hpp>

class Test;

using TestHandle = Nz::ObjectHandle<Test>;

struct Test : public Nz::HandledObject<Test>
{
	Test(int value) :
	i(value)
	{
	}

	int i;
};

SCENARIO("Handle", "[CORE][HandledObject][ObjectHandle]")
{
	GIVEN("One test with two handles")
	{
		int defaultValue = 1;
		Test test(defaultValue);

		Nz::ObjectHandle<Test> handle1 = test.CreateHandle();
		Nz::ObjectHandle<Test> handle2 = test.CreateHandle();

		WHEN("We modify from one")
		{
			const int newI = 2;
			handle1->i = newI;

			THEN("The other one should also be modified")
			{
				REQUIRE(handle2->i == newI);
			}
		}

		WHEN("We copy construct")
		{
			Test other(test);
			Nz::ObjectHandle<Test> otherHandle = other.CreateHandle();

			THEN("Handles should point to 1")
			{
				CHECK(handle1->i == defaultValue);
				CHECK(handle2->i == defaultValue);
				CHECK(otherHandle->i == defaultValue);
				CHECK(handle2.GetObject() == &test);
				CHECK(otherHandle.GetObject() == &other);
			}
		}

		WHEN("We move construct")
		{
			Test other(std::move(test));
			Nz::ObjectHandle<Test> otherHandle = other.CreateHandle();

			THEN("Handles should point to 1")
			{
				CHECK(handle1->i == defaultValue);
				CHECK(handle2->i == defaultValue);
				CHECK(otherHandle->i == defaultValue);
				CHECK(handle1.GetObject() == &other);
			}
		}

		WHEN("We copy assign")
		{
			int copyValue = 3;
			Test other(copyValue);
			Nz::ObjectHandle<Test> otherHandle = other.CreateHandle();
			test = other;

			THEN("Handles should point to 3")
			{
				CHECK(handle1->i == copyValue);
				CHECK(handle2->i == copyValue);
				CHECK(otherHandle->i == copyValue);
				CHECK(handle1.GetObject() == &test);
				CHECK(otherHandle.GetObject() == &other);
			}
		}

		WHEN("We move assign")
		{
			int moveValue = 4;
			Test other(moveValue);
			Nz::ObjectHandle<Test> otherHandle = other.CreateHandle();
			test = std::move(other);

			THEN("Handles to previous objects should be invalid")
			{
				CHECK_FALSE(handle1.IsValid());
				CHECK_FALSE(handle2.IsValid());
			}

			THEN("Handles should point to 4")
			{
				CHECK(otherHandle.GetObject() == &test);
				CHECK(otherHandle->i == moveValue);
			}
		}
	}

	GIVEN("One handle pointing to a default test")
	{
		Test test(1);
		Nz::ObjectHandle<Test> invalidHandle(&test);

		WHEN("We bind it to a HandledObject which is going to die")
		{
			{
				Test dyingTest(5);
				invalidHandle.Reset(&dyingTest);
			}

			THEN("It should not be valid")
			{
				REQUIRE(!invalidHandle.IsValid());
			}
		}
	}

	GIVEN("Two handle pointing to two different tests")
	{
		Test test1(1);
		Nz::ObjectHandle<Test> test1Handle = test1.CreateHandle();
		Test test2(2);
		Nz::ObjectHandle<Test> test2Handle = test2.CreateHandle();

		WHEN("We swap their content")
		{
			test1Handle.Swap(test2Handle);

			THEN("They should be pointing to the correct one")
			{
				CHECK(test1Handle.GetObject() == &test2);
				CHECK(test2Handle.GetObject() == &test1);
			}
		}
	}
}

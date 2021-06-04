#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <Catch/catch.hpp>

struct ObjectHandle_Test : public Nz::HandledObject<ObjectHandle_Test>
{
	ObjectHandle_Test(int value) :
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
		ObjectHandle_Test test(defaultValue);

		Nz::ObjectHandle<ObjectHandle_Test> handle1 = test.CreateHandle();
		Nz::ObjectHandle<ObjectHandle_Test> handle2 = test.CreateHandle();

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
			ObjectHandle_Test other(test);
			Nz::ObjectHandle<ObjectHandle_Test> otherHandle = other.CreateHandle();

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
			ObjectHandle_Test other(std::move(test));
			Nz::ObjectHandle<ObjectHandle_Test> otherHandle = other.CreateHandle();

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
			ObjectHandle_Test other(copyValue);
			Nz::ObjectHandle<ObjectHandle_Test> otherHandle = other.CreateHandle();
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
			ObjectHandle_Test other(moveValue);
			Nz::ObjectHandle<ObjectHandle_Test> otherHandle = other.CreateHandle();
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
		ObjectHandle_Test test(1);
		Nz::ObjectHandle<ObjectHandle_Test> invalidHandle(&test);

		WHEN("We bind it to a HandledObject which is going to die")
		{
			{
				ObjectHandle_Test dyingTest(5);
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
		ObjectHandle_Test test1(1);
		Nz::ObjectHandle<ObjectHandle_Test> test1Handle = test1.CreateHandle();
		ObjectHandle_Test test2(2);
		Nz::ObjectHandle<ObjectHandle_Test> test2Handle = test2.CreateHandle();

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

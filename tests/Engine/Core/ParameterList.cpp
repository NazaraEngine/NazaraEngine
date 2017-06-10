#include <Nazara/Core/ParameterList.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Core/String.hpp>

SCENARIO("ParameterList", "[CORE][PARAMETERLIST]")
{
	GIVEN("An empty ParameterList")
	{
		Nz::ParameterList parameterList;

		WHEN("We add String 'string'")
		{
			Nz::String string("string");
			parameterList.SetParameter("string", string);

			THEN("We can get it back")
			{
				Nz::String newString;
				REQUIRE(parameterList.GetStringParameter("string", &newString));
				REQUIRE(newString == string);
			}
		}

		WHEN("We add Float '3.f'")
		{
			double fl = 3.f;
			parameterList.SetParameter("double", fl);

			THEN("We can get it back")
			{
				double newFl;
				REQUIRE(parameterList.GetDoubleParameter("double", &newFl));
				REQUIRE(newFl == fl);
			}
		}

		WHEN("We add Pointer to stack value")
		{
			int stackValue = 3;
			void* ptrToStackValue = &stackValue; // Ugly conversion
			parameterList.SetParameter("ptr", ptrToStackValue);

			THEN("We can get it back")
			{
				void* newPtrToStackValue = nullptr;
				REQUIRE(parameterList.GetPointerParameter("ptr", &newPtrToStackValue));
				REQUIRE(newPtrToStackValue == ptrToStackValue);
			}
		}
	}
}

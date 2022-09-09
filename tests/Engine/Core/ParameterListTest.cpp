#include <Nazara/Core/ParameterList.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

void nullAction(void*)
{
}

SCENARIO("ParameterList", "[CORE][PARAMETERLIST]")
{
	GIVEN("An empty ParameterList")
	{
		Nz::ParameterList parameterList;

		WHEN("We add Bool 'true' and analogous")
		{
			bool boolean = true;
			parameterList.SetParameter("bool", boolean);

			long long intTrue = 1;
			parameterList.SetParameter("intTrue", intTrue);
			long long intFalse = 0;
			parameterList.SetParameter("intFalse", intFalse);

			std::string strTrue = "true";
			parameterList.SetParameter("strTrue", strTrue);
			std::string strFalse = "false";
			parameterList.SetParameter("strFalse", strFalse);

			THEN("We can get it back")
			{
				CHECK(parameterList.GetBooleanParameter("bool").GetValue() == true);
			}

			THEN("Conversion from int to bool should also work if strict mode is disabled")
			{
				CHECK(parameterList.GetBooleanParameter("intTrue", false).GetValue() == true);
				CHECK(parameterList.GetBooleanParameter("intFalse", false).GetValue() == false);
			}

			THEN("Conversion from str to bool should also work if strict mode is disabled")
			{
				CHECK(parameterList.GetBooleanParameter("strTrue", false).GetValue() == true);
				CHECK(parameterList.GetBooleanParameter("strFalse", false).GetValue() == false);
			}
		}

		WHEN("We add Color 'rgb(1, 2, 3)'")
		{
			Nz::Color rgb(1, 2, 3);
			parameterList.SetParameter("color", rgb);

			THEN("We can get it back")
			{
				CHECK(parameterList.GetColorParameter("color").GetValue() == rgb);
			}
		}

		WHEN("We add Double '3.0' and analogous")
		{
			double fl = 3.0;
			parameterList.SetParameter("double", fl);

			long long intDouble = 3;
			parameterList.SetParameter("intDouble", intDouble);

			std::string strDouble = "3.0";
			parameterList.SetParameter("strDouble", strDouble);

			THEN("We can get it back")
			{
				CHECK(parameterList.GetDoubleParameter("double").GetValue() == fl);
			}

			THEN("Conversion from int to double should also work if strict mode is disabled")
			{
				CHECK(parameterList.GetDoubleParameter("intDouble", false).GetValue() == fl);
			}

			THEN("Conversion from string to double should also work if strict mode is disabled")
			{
				CHECK(parameterList.GetDoubleParameter("strDouble", false).GetValue() == fl);
			}
		}

		WHEN("We add Int '3' and analogous")
		{
			long long i = 3;
			parameterList.SetParameter("int", i);

			bool trueInt = 1;
			parameterList.SetParameter("trueInt", trueInt);
			bool falseInt = 0;
			parameterList.SetParameter("falseInt", falseInt);

			double doubleInt = 3;
			parameterList.SetParameter("doubleInt", doubleInt);

			std::string strInt = "3";
			parameterList.SetParameter("strInt", strInt);

			THEN("We can get it back")
			{
				CHECK(parameterList.GetIntegerParameter("int").GetValue() == i);
			}

			THEN("Conversion from bool to int should also work if strict mode is disabled")
			{
				CHECK(parameterList.GetIntegerParameter("trueInt", false).GetValue() == trueInt);
				CHECK(parameterList.GetIntegerParameter("falseInt", false).GetValue() == falseInt);
			}

			THEN("Conversion from double to int should also work if strict mode is disabled")
			{
				CHECK(parameterList.GetIntegerParameter("doubleInt", false).GetValue() == i);
			}

			THEN("Conversion from string to int should also work if strict mode is disabled")
			{
				CHECK(parameterList.GetIntegerParameter("strInt", false).GetValue() == i);
			}
		}

		WHEN("We add String 'string' and analogous")
		{
			std::string string("string");
			parameterList.SetParameter("string", string);

			bool trueString = 1;
			parameterList.SetParameter("trueString", trueString);
			bool falseString = 0;
			parameterList.SetParameter("falseString", falseString);

			Nz::Color colorString(1, 2, 3);
			parameterList.SetParameter("colorString", colorString);

			double doubleString = 3.0;
			parameterList.SetParameter("doubleString", doubleString);

			long long intString = 3;
			parameterList.SetParameter("intString", intString);

			THEN("We can get it back")
			{
				CHECK(parameterList.GetStringParameter("string").GetValue() == string);
				CHECK(parameterList.GetStringViewParameter("string").GetValue() == string);
			}

			THEN("Conversion from bool to str should also work if strict mode is disabled")
			{
				CHECK(parameterList.GetStringParameter("trueString", false).GetValue() == "true");
				CHECK(parameterList.GetStringParameter("falseString", false).GetValue() == "false");
				CHECK(parameterList.GetStringViewParameter("trueString", false).GetValue() == "true");
				CHECK(parameterList.GetStringViewParameter("falseString", false).GetValue() == "false");
			}

			THEN("Conversion from color to string should also work if strict mode is disabled")
			{
				CHECK(parameterList.GetStringParameter("colorString", false).GetValue() == colorString.ToString());
			}

			THEN("Conversion from string to double should also work if strict mode is disabled")
			{
				CHECK(parameterList.GetStringParameter("doubleString", false).GetValue() == "3.000000");
			}

			THEN("Conversion from string to int should also work if strict mode is disabled")
			{
				CHECK(parameterList.GetStringParameter("intString", false).GetValue() == "3");
			}
		}

		WHEN("We add Pointer to stack value")
		{
			int stackValue = 3;
			void* ptrToStackValue = &stackValue; // Ugly conversion
			parameterList.SetParameter("ptr", ptrToStackValue);

			THEN("We can get it back")
			{
				CHECK(parameterList.GetPointerParameter("ptr").GetValue() == ptrToStackValue);
			}
		}

		WHEN("We set our own data")
		{
			struct Data {
				int i;
				float f;
			};

			Data data{ 1, 3.f };
			parameterList.SetParameter("userData", &data, nullAction);

			THEN("We can get it back")
			{
				void* ptrToData;

				CHECK_NOTHROW(ptrToData = parameterList.GetUserdataParameter("userData").GetValue());
				Data* dataPtr = static_cast<Data*>(ptrToData);
				CHECK(dataPtr->i == data.i);
				CHECK(dataPtr->f == data.f);
			}
		}
	}

	GIVEN("A parameter list with some values")
	{
		Nz::ParameterList parameterList;

		long long i = 3;
		parameterList.SetParameter("i", i);
		double d = 1.0;
		parameterList.SetParameter("d", d);

		parameterList.SetParameter("toaster");
		parameterList.SetParameter("str", "ing");

		WHEN("We remove two elements")
		{
			CHECK(parameterList.HasParameter("i"));
			CHECK(parameterList.HasParameter("toaster"));

			parameterList.RemoveParameter("i");
			parameterList.RemoveParameter("toaster");

			THEN("They do not exist anymore")
			{
				CHECK(!parameterList.HasParameter("i"));
				CHECK(!parameterList.HasParameter("toaster"));
			}
		}

		WHEN("We copy this list")
		{
			Nz::ParameterList copy = parameterList;

			THEN("It has the same elements")
			{
				CHECK(parameterList.HasParameter("i"));
				CHECK(parameterList.HasParameter("d"));
				CHECK(parameterList.HasParameter("toaster"));
				CHECK(parameterList.HasParameter("str"));
			}
		}
	}
}

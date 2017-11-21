#include <Nazara/Core/ParameterList.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Core/String.hpp>

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

			Nz::String strTrue = "true";
			parameterList.SetParameter("strTrue", strTrue);
			Nz::String strFalse = "false";
			parameterList.SetParameter("strFalse", strFalse);

			THEN("We can get it back")
			{
				bool retrievedValue = false;
				CHECK(parameterList.GetBooleanParameter("bool", &retrievedValue));
				CHECK(retrievedValue == boolean);
			}

			THEN("Conversion from int to bool should also work")
			{
				bool retrievedValue = false;
				CHECK(parameterList.GetBooleanParameter("intTrue", &retrievedValue));
				CHECK(retrievedValue);
				CHECK(parameterList.GetBooleanParameter("intFalse", &retrievedValue));
				CHECK(!retrievedValue);
			}

			THEN("Conversion from str to bool should also work")
			{
				bool retrievedValue = false;
				CHECK(parameterList.GetBooleanParameter("strTrue", &retrievedValue));
				CHECK(retrievedValue);
				CHECK(parameterList.GetBooleanParameter("strFalse", &retrievedValue));
				CHECK(!retrievedValue);
			}
		}

		WHEN("We add Color 'rgb(1, 2, 3)'")
		{
			Nz::Color rgb(1, 2, 3);
			parameterList.SetParameter("color", rgb);

			THEN("We can get it back")
			{
				Nz::Color retrievedColor;
				CHECK(parameterList.GetColorParameter("color", &retrievedColor));
				CHECK(retrievedColor == rgb);
			}
		}

		WHEN("We add Double '3.0' and analogous")
		{
			double fl = 3.0;
			parameterList.SetParameter("double", fl);

			long long intDouble = 3;
			parameterList.SetParameter("intDouble", intDouble);

			Nz::String strDouble = "3.0";
			parameterList.SetParameter("strDouble", strDouble);

			THEN("We can get it back")
			{
				double retrievedValue;
				CHECK(parameterList.GetDoubleParameter("double", &retrievedValue));
				CHECK(retrievedValue == fl);
			}

			THEN("Conversion from int to double should also work")
			{
				double retrievedValue;
				CHECK(parameterList.GetDoubleParameter("intDouble", &retrievedValue));
				CHECK(retrievedValue == fl);
			}

			THEN("Conversion from string to double should also work")
			{
				double retrievedValue;
				CHECK(parameterList.GetDoubleParameter("strDouble", &retrievedValue));
				CHECK(retrievedValue == fl);
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

			Nz::String strInt = "3";
			parameterList.SetParameter("strInt", strInt);

			THEN("We can get it back")
			{
				long long retrievedValue;
				CHECK(parameterList.GetIntegerParameter("int", &retrievedValue));
				CHECK(retrievedValue == i);
			}

			THEN("Conversion from bool to int should also work")
			{
				long long retrievedValue;
				CHECK(parameterList.GetIntegerParameter("trueInt", &retrievedValue));
				CHECK(retrievedValue == trueInt);
				CHECK(parameterList.GetIntegerParameter("falseInt", &retrievedValue));
				CHECK(retrievedValue == falseInt);
			}

			THEN("Conversion from double to int should also work")
			{
				long long retrievedValue;
				CHECK(parameterList.GetIntegerParameter("doubleInt", &retrievedValue));
				CHECK(retrievedValue == i);
			}

			THEN("Conversion from string to int should also work")
			{
				long long retrievedValue;
				CHECK(parameterList.GetIntegerParameter("strInt", &retrievedValue));
				CHECK(retrievedValue == i);
			}
		}

		WHEN("We add String 'string' and analogous")
		{
			Nz::String string("string");
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
				Nz::String newString;
				CHECK(parameterList.GetStringParameter("string", &newString));
				CHECK(newString == string);
			}

			THEN("Conversion from bool to str should also work")
			{
				Nz::String retrievedValue;
				CHECK(parameterList.GetStringParameter("trueString", &retrievedValue));
				CHECK(retrievedValue == "true");
				CHECK(parameterList.GetStringParameter("falseString", &retrievedValue));
				CHECK(retrievedValue == "false");
			}

			THEN("Conversion from color to string should also work")
			{
				Nz::String retrievedValue;
				CHECK(parameterList.GetStringParameter("colorString", &retrievedValue));
				CHECK(retrievedValue == colorString.ToString());
			}

			THEN("Conversion from string to double should also work")
			{
				Nz::String retrievedValue;
				CHECK(parameterList.GetStringParameter("doubleString", &retrievedValue));
				CHECK(retrievedValue == "3");
			}

			THEN("Conversion from string to int should also work")
			{
				Nz::String retrievedValue;
				CHECK(parameterList.GetStringParameter("intString", &retrievedValue));
				CHECK(retrievedValue == "3");
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
				CHECK(parameterList.GetPointerParameter("ptr", &newPtrToStackValue));
				CHECK(newPtrToStackValue == ptrToStackValue);
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
				Data retrievedValue;
				void* ptrToData = &retrievedValue;

				CHECK(parameterList.GetUserdataParameter("userData", &ptrToData));
				Data* dataPtr = reinterpret_cast<Data*>(ptrToData);
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

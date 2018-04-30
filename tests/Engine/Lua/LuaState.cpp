#include <Nazara/Lua/LuaInstance.hpp>
#include <Catch/catch.hpp>

static int counter(lua_State* L)
{
	Nz::LuaInstance& luaInstance = Nz::LuaInstance::GetInstance(L);
	double val = luaInstance.ToNumber(luaInstance.GetIndexOfUpValue(1));
	luaInstance.PushNumber(++val);
	luaInstance.PushValue(-1);
	luaInstance.Replace(luaInstance.GetIndexOfUpValue(1));
	return 1;
}

struct TestLuaState
{
	int a = 3;
	float b = 5.f;
};

struct TestMetaTable
{
	float x = 1.f;
	float y = 2.f;
};

inline int LuaImplReplyVal(const Nz::LuaState& state, TestLuaState&& val, Nz::TypeTag<TestLuaState>)
{
	state.PushTable();
	state.PushField("a", val.a);
	state.PushField("b", val.b);

	return 1;
}

inline unsigned int LuaImplQueryArg(const Nz::LuaState& state, int index, TestLuaState* arg, Nz::TypeTag<TestLuaState>)
{
	state.CheckType(index, Nz::LuaType_Table);

	arg->a = state.CheckField<int>("a", index);
	arg->b = state.CheckField<float>("b", index);

	return 1;
}

SCENARIO("LuaState", "[LUA][LUASTATE]")
{
	GIVEN("One lua instance")
	{
		Nz::LuaInstance luaInstance;
		luaInstance.LoadLibraries(Nz::LuaLib_Math);

		WHEN("We push different primitive types")
		{
			bool booleanValue = true;
			long long integerValue = 5LL;
			double doubleValue = -55.0;
			const char* stringValue = "test";
			Nz::String nazaraValue = "Nazara";
			luaInstance.PushBoolean(booleanValue);
			luaInstance.PushInteger(integerValue);
			luaInstance.PushNil();
			luaInstance.PushNumber(doubleValue);
			luaInstance.PushString(stringValue);
			luaInstance.PushString(nazaraValue);

			THEN("We should be able to retrieve them")
			{
				CHECK(luaInstance.CheckBoolean(1) == booleanValue);
				CHECK(luaInstance.CheckInteger(2) == integerValue);
				bool succeeded = false;
				CHECK(luaInstance.ToInteger(2, &succeeded) == integerValue);
				CHECK(succeeded);
				CHECK(luaInstance.ToPointer(3) == nullptr);
				CHECK(luaInstance.CheckNumber(4) == Approx(doubleValue));
				succeeded = false;
				CHECK(luaInstance.ToNumber(4, &succeeded) == Approx(doubleValue));
				CHECK(succeeded);
				CHECK(luaInstance.CheckString(5) == std::string(stringValue));
				CHECK(luaInstance.CheckString(6) == nazaraValue);
				std::size_t length = 0;
				CHECK(luaInstance.ToString(6, &length) == nazaraValue);
				CHECK(length == nazaraValue.GetSize());
			}
		}

		WHEN("We use basic operations")
		{
			luaInstance.PushInteger(1);
			luaInstance.PushInteger(2);

			THEN("We should behave normally")
			{
				CHECK(luaInstance.Compare(1, 2, Nz::LuaComparison_Less));
				luaInstance.Compute(Nz::LuaOperation_Substraction);
				CHECK(luaInstance.ToInteger(1) == -1);
			}
		}

		WHEN("We manipulate the stack")
		{
			Nz::String stringValue = "hello";
			luaInstance.PushBoolean(true);
			luaInstance.PushNumber(10.0);
			luaInstance.PushNil();
			luaInstance.PushString(stringValue);
			/* true 10.0 nil hello */

			THEN("These effects are expected")
			{
				luaInstance.PushValue(-4);
				/* true 10.0 nil hello true */
				CHECK(luaInstance.CheckBoolean(5));

				luaInstance.Replace(3);
				/* true 10.0 true hello */
				CHECK(luaInstance.CheckBoolean(3));

				luaInstance.Remove(-2);
				/* true 10.0 hello */
				CHECK(luaInstance.CheckString(3) == stringValue);

				luaInstance.Pop(2);
				/* true */
				CHECK_FALSE(luaInstance.IsValid(2));
			}
		}

		WHEN("We try the CFunction")
		{
			double counterValue = 55.0;
			luaInstance.PushFunction([=](Nz::LuaState& s) -> int {
				s.PushNumber(counterValue);
				s.PushCFunction(&counter, 1);
				return 1;
			});

			THEN("We can call them")
			{
				luaInstance.Call(0); // We call our counter creator
				luaInstance.Call(0); // We call our counter, which increments the value
				CHECK(luaInstance.ToNumber(0) == Approx(counterValue + 1.0));
			}
		}

		WHEN("We push our user type locally")
		{
			luaInstance.Push(TestLuaState());

			THEN("We can retrieve it")
			{
				int index = 1;
				TestLuaState popped = luaInstance.Check<TestLuaState>(&index);
				CHECK(popped.a == 3);
				CHECK(popped.b == Approx(5.0));
			}
		}

		WHEN("We push our user type globally")
		{
			luaInstance.PushGlobal("TestLuaState", TestLuaState());

			THEN("We can retrieve it")
			{
				TestLuaState popped = luaInstance.CheckGlobal<TestLuaState>("TestLuaState");
				CHECK(popped.a == 3);
				CHECK(popped.b == Approx(5.0));
			}
		}

		WHEN("We define a lua function")
		{
			luaInstance.Execute(R"(
				function f (x, y)
					return (x^2 * math.sin(y))/(1 - x)
				end
			)");

			THEN("We can call it from the code")
			{
				REQUIRE(luaInstance.GetGlobal("f") == Nz::LuaType_Function);
				luaInstance.Push(3.0, 2.0);
				luaInstance.Call(2, 1);
				CHECK(luaInstance.ToNumber(1) == Approx(-4.09).margin(0.1));
			}
		}

		WHEN("We push a std::vector locally")
		{
			std::vector<int> vec { 1, 5, -8, 6, -4 };
			luaInstance.Push(vec);

			THEN("We can retrieve it with correct values")
			{
				int index = 1;
				std::vector<int> otherVec = luaInstance.Check<std::vector<int>>(&index);

				for (std::size_t i {}; i < otherVec.size(); ++i)
					CHECK(otherVec[i] == vec[i]);
			}
		}

		WHEN("We push a std::vector globally")
		{
			std::vector<int> vec { 1, 5, -8, 6, -4 };
			luaInstance.PushGlobal("vector", vec);

			THEN("We can retrieve it with correct values")
			{
				std::vector<int> otherVec = luaInstance.CheckGlobal<std::vector<int>>("vector");

				for (std::size_t i {}; i < otherVec.size(); ++i)
					CHECK(otherVec[i] == vec[i]);
			}
		}
	}
}

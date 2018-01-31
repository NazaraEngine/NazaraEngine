#include <Nazara/Lua/LuaClass.hpp>
#include <Catch/catch.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/ObjectHandle.hpp>

class Test
{
	public:
		Test() = default;
		Test(const Test& other) = default;
		Test& operator=(const Test& other) = default;
		virtual ~Test() = default;

		Test(int i, bool j = false) :
		m_i(i),
		m_j(j)
		{
		}

		virtual int GetI() const
		{
			return m_i;
		}

		bool GetJ() const
		{
			return m_j;
		}

		int GetDefault(int defaultValue = 0)
		{
			return defaultValue;
		}

		static int StaticMethodWithArguments(int a, int b)
		{
			return a + b;
		}

	private:
		int m_i;
		bool m_j;
};

class InheritTest : public Test
{
	public:
		InheritTest() :
			Test(5, true)
		{
		}

		int GetI() const override
		{
			return Test::GetI() + 3;
		}
};

class TestWithHandle : public Nz::HandledObject<TestWithHandle>
{
	public:
		int GetI() const
		{
			return m_i;
		}

		int GetDefault(int defaultValue = 0)
		{
			return defaultValue;
		}

	private:
		int m_i = 8;
};

inline unsigned int LuaImplQueryArg(const Nz::LuaState& instance, int index, Test* arg, Nz::TypeTag<Test>)
{
	REQUIRE(instance.IsOfType(index, "Test"));
	*arg = *static_cast<Test*>(instance.ToUserdata(index));
	return 1;
}

inline unsigned int LuaImplQueryArg(const Nz::LuaState& instance, int index, Nz::ObjectHandle<TestWithHandle>* arg, Nz::TypeTag<Nz::ObjectHandle<TestWithHandle>>)
{
	REQUIRE(instance.IsOfType(index, "TestWithHandle"));
	*arg = *static_cast<Nz::ObjectHandle<TestWithHandle>*>(instance.ToUserdata(index));
	return 1;
}

inline unsigned int LuaImplQueryArg(const Nz::LuaState& instance, int index, InheritTest* arg, Nz::TypeTag<InheritTest>)
{
	REQUIRE(instance.IsOfType(index, "InheritTest"));
	*arg = *static_cast<InheritTest*>(instance.ToUserdata(index));
	return 1;
}

SCENARIO("LuaClass", "[LUA][LUACLASS]")
{
	GIVEN("One lua class for our Test class")
	{
		Nz::LuaInstance luaInstance;
		Nz::LuaClass<Test> test;
		Nz::LuaClass<InheritTest> inheritTest;
		using TestHandle = Nz::ObjectHandle<TestWithHandle>;
		Nz::LuaClass<TestHandle> testHandle;

		WHEN("We bind the methods")
		{
			test.Reset("Test");

			test.BindDefaultConstructor();

			test.SetConstructor([] (Nz::LuaState& lua, Test* instance, std::size_t argumentCount)
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 2U);

				int argIndex = 1;
				switch (argCount)
				{
					case 1:
					{
						int iValue = lua.Check<int>(&argIndex, 0);

						Nz::PlacementNew(instance, iValue);
						return true;
					}

					case 2:
					{
						int iValue = lua.Check<int>(&argIndex, 0);
						bool j = lua.Check<bool>(&argIndex, false);

						Nz::PlacementNew(instance, iValue, j);
						return true;
					}
				}

				lua.Error("No matching overload for Test constructor");
				return false;
			});

			test.BindMethod("GetI", &Test::GetI);
			test.BindMethod("GetJ", &Test::GetJ);
			test.BindMethod("GetDefault", &Test::GetDefault, 0);

			test.BindStaticMethod("StaticMethodWithArguments", [] (Nz::LuaState& state) -> int
			{
				int argIndex = 1;
				int result = Test::StaticMethodWithArguments(state.Check<int>(&argIndex), state.Check<int>(&argIndex));

				state.Push(result);
				return 1;
			});

			test.Register(luaInstance);

			THEN("We should be able to call them")
			{
				int value = 1;
				int staticResult = value + value;

				luaInstance.PushFunction([=](Nz::LuaState& state) -> int
				{
					int argIndex = 1;
					Test result = state.Check<Test>(&argIndex);
					CHECK(result.GetI() == value);
					CHECK_FALSE(result.GetJ());
					return 1;
				});
				luaInstance.SetGlobal("CheckTest");

				luaInstance.PushFunction([=](Nz::LuaState& state) -> int
				{
					int argIndex = 1;
					int result = state.Check<int>(&argIndex);
					CHECK(result == staticResult);
					return 1;
				});
				luaInstance.SetGlobal("CheckStatic");

				luaInstance.PushFunction([=](Nz::LuaState& state) -> int
				{
					int argIndex = 1;
					Test result = state.Check<Test>(&argIndex);
					CHECK(result.GetI() == staticResult);
					CHECK(result.GetJ());
					return 1;
				});
				luaInstance.SetGlobal("CheckFinalTest");

				REQUIRE(luaInstance.ExecuteFromFile("resources/Engine/Lua/LuaClass.lua"));
				REQUIRE(luaInstance.GetGlobal("test_Test") == Nz::LuaType_Function);
				luaInstance.Call(0);
			}

			AND_THEN("With a subclass")
			{
				inheritTest.Reset("InheritTest");

				inheritTest.Inherit(test);
				inheritTest.BindDefaultConstructor();

				inheritTest.Register(luaInstance);

				luaInstance.PushFunction([=](Nz::LuaState& state) -> int
				{
					int argIndex = 1;
					InheritTest result = state.Check<InheritTest>(&argIndex);
					CHECK(result.GetI() == 8);
					CHECK(result.GetJ());
					return 1;
				});
				luaInstance.SetGlobal("CheckInheritTest");

				REQUIRE(luaInstance.ExecuteFromFile("resources/Engine/Lua/LuaClass.lua"));
				REQUIRE(luaInstance.GetGlobal("test_InheritTest") == Nz::LuaType_Function);
				luaInstance.Call(0);
			}
		}

		WHEN("We bind the object with Handle")
		{
			int defaultValue = 5;

			testHandle.Reset("TestHandle");
			testHandle.BindMethod("IsValid", &TestHandle::IsValid);
			testHandle.BindMethod("GetI", &TestWithHandle::GetI);
			testHandle.BindMethod("GetDefault", &TestWithHandle::GetDefault, defaultValue);
			testHandle.Register(luaInstance);

			THEN("We can ensure the following properties")
			{
				luaInstance.PushFunction([=](Nz::LuaState& state) -> int
				{
					int argIndex = 1;
					TestHandle result = state.Check<TestHandle>(&argIndex);
					CHECK(result->GetI() == 8);
					CHECK(result->GetDefault() == defaultValue);
					return 1;
				});
				luaInstance.SetGlobal("CheckTestHandle");

				REQUIRE(luaInstance.ExecuteFromFile("resources/Engine/Lua/LuaClass.lua"));
				REQUIRE(luaInstance.GetGlobal("test_TestHandle") == Nz::LuaType_Function);
				luaInstance.Call(0);
			}
		}
	}
}

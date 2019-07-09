#include <Nazara/Lua/LuaInstance.hpp>
#include <Catch/catch.hpp>
#include <iostream>

SCENARIO("LuaInstance", "[LUA][LUAINSTANCE]")
{
	GIVEN("One lua instance")
	{
		Nz::LuaInstance luaInstance;

		WHEN("We set memory constraint")
		{
			luaInstance.SetMemoryLimit(1'000);

			THEN("If we excess memory, it should crash")
			{
				REQUIRE_THROWS_WITH(luaInstance.Execute(R"(
						global t = {}
						for 1,10000000 do
							t[i] = i
						end
					)"), Catch::Matchers::Contains("memory"));
			}
		}

		WHEN("We set time constraint")
		{
			luaInstance.SetTimeLimit(10);

			THEN("If we excess time, it should produce an error")
			{
				CHECK(!luaInstance.Execute(R"(
					function ack(M,N)
						if M == 0 then return N + 1 end
						if N == 0 then return ack(M-1,1) end
						return ack(M-1,ack(M, N-1))
					end
					ack(100, 100)
				)"));

				REQUIRE_THAT(luaInstance.GetLastError().ToStdString(), Catch::Matchers::Contains("time"));
			}
		}
	}

	GIVEN("Two instances")
	{
		Nz::LuaInstance luaInstance;

		int memoryLimit = 10'000;
		int timeLimit = 1'000;
		luaInstance.SetMemoryLimit(memoryLimit);
		luaInstance.SetTimeLimit(timeLimit);

		int value = 5;
		luaInstance.PushInteger(value);

		WHEN("We use move constructor")
		{
			Nz::LuaInstance movedInstance(std::move(luaInstance));

			THEN("We should be able to retrieve the value")
			{
				CHECK(movedInstance.CheckInteger(1) == value);
				CHECK(movedInstance.GetMemoryLimit() == memoryLimit);
				CHECK(movedInstance.GetTimeLimit() == timeLimit);
			}
		}

		WHEN("We use move assignment")
		{
			Nz::LuaInstance movedInstance;
			movedInstance.PushInteger(value + 1);
			movedInstance = std::move(luaInstance);

			THEN("We should be able to retrieve the value")
			{
				CHECK(movedInstance.CheckInteger(1) == value);
				CHECK(movedInstance.GetMemoryLimit() == memoryLimit);
				CHECK(movedInstance.GetTimeLimit() == timeLimit);
			}
		}
	}
}

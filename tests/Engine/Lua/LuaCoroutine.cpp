#include <Nazara/Lua/LuaCoroutine.hpp>
#include <Catch/catch.hpp>
#include <Nazara/Lua/LuaInstance.hpp>

SCENARIO("LuaCoroutine", "[LUA][LUACOROUTINE]")
{
	GIVEN("One lua with coroutine")
	{
		Nz::LuaInstance luaInstance;
		luaInstance.LoadLibraries(Nz::LuaLib_Coroutine);

		REQUIRE(luaInstance.ExecuteFromFile("resources/Engine/Lua/LuaCoroutine.lua"));

		Nz::LuaCoroutine coroutine = luaInstance.NewCoroutine();
		REQUIRE(coroutine.GetGlobal("infinite") == Nz::LuaType_Function);
		coroutine.PushInteger(4);
		CHECK(coroutine.Call(1));

		coroutine.Resume();
		CHECK(coroutine.CheckInteger(1) == 1);
		coroutine.Resume();
		CHECK(coroutine.CheckInteger(1) == -1);
		coroutine.Resume();
		CHECK(coroutine.CheckInteger(1) == 0);
		coroutine.Resume();
		CHECK(coroutine.CheckInteger(1) == 1);

		coroutine.Resume();
		CHECK_FALSE(coroutine.CanResume());
	}
}

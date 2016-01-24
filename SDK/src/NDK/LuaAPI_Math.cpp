// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/LuaAPI.hpp>
#include <Nazara/Math.hpp>
#include <Nazara/Lua/LuaClass.hpp>

namespace Ndk
{
	void LuaAPI::Register_Math(Nz::LuaInstance& instance)
	{
		/*********************************** Nz::Vector2 **********************************/
		Nz::LuaClass<Nz::Vector2d> vector2dClass("Vector2");

		vector2dClass.SetConstructor([](Nz::LuaInstance& lua) -> Nz::Vector2d* 
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 2U);
			switch (argCount)
			{
				case 0:
				case 2:
					return new Nz::Vector2d(lua.CheckNumber(1, 0.0), lua.CheckNumber(2, 0.0));

				case 1:
				{
					if (lua.IsOfType(1, Nz::LuaType_Number))
						return new Nz::Vector2d(lua.CheckNumber(1));
					else if (lua.IsOfType(1, "Vector2"))
						return new Nz::Vector2d(*(*static_cast<Nz::Vector2d*>(lua.ToUserdata(1))));

					break;
				}
			}

			lua.Error("No matching overload for Vector2 constructor");
			return nullptr;
		});

		vector2dClass.SetMethod("__tostring", &Nz::Vector2d::ToString);

		vector2dClass.SetGetter([](Nz::LuaInstance& lua, Nz::Vector2d& instance)
		{
			switch (lua.GetType(1))
			{
				case Nz::LuaType_Number:
					lua.Push(instance[lua.CheckInteger(1)]);
					return true;

				case Nz::LuaType_String:
				{
					std::size_t length;
					const char* xy = lua.CheckString(1, &length);

					if (length != 1)
						break;

					switch (xy[0])
					{
						case 'x':
							lua.Push(instance.x);
							return true;

						case 'y':
							lua.Push(instance.y);
							return true;
					}
					break;
				}
			}

			return false;
		});

		vector2dClass.SetSetter([](Nz::LuaInstance& lua, Nz::Vector2d& instance)
		{
			switch (lua.GetType(1))
			{
				case Nz::LuaType_Number:
				{
					long long index = lua.CheckInteger(1);
					if (index < 1 || index > 2)
						return false;

					instance[index] = lua.CheckNumber(2);
					return true;
				}

				case Nz::LuaType_String:
				{
					std::size_t length;
					const char* xy = lua.CheckString(1, &length);

					if (length != 1)
						break;

					double value = lua.CheckNumber(2);

					switch (xy[0])
					{
						case 'x':
							instance.x = value;
							return true;

						case 'y':
							instance.y = value;
							return true;
					}
					break;
				}
			}

			return false;
		});

		vector2dClass.Register(instance);

		/*********************************** Nz::Vector3 **********************************/
		Nz::LuaClass<Nz::Vector3d> vector3dClass("Vector3");

		vector3dClass.SetConstructor([] (Nz::LuaInstance& lua) -> Nz::Vector3d*
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 3U);
			switch (argCount)
			{
				case 0:
				case 3:
					return new Nz::Vector3d(lua.CheckNumber(1, 0.0), lua.CheckNumber(2, 0.0), lua.CheckNumber(3, 0.0));

				case 1:
				{
					if (lua.IsOfType(1, Nz::LuaType_Number))
						return new Nz::Vector3d(lua.CheckNumber(1), *static_cast<Nz::Vector2d*>(lua.ToUserdata(1)));
					else if (lua.IsOfType(1, "Vector2"))
						return new Nz::Vector3d(*(*static_cast<Nz::Vector2d*>(lua.ToUserdata(1))));
					else if (lua.IsOfType(1, "Vector3"))
						return new Nz::Vector3d(*(*static_cast<Nz::Vector3d*>(lua.ToUserdata(1))));

					break;
				}

				case 2:
				{
					if (lua.IsOfType(1, Nz::LuaType_Number))
						return new Nz::Vector3d(lua.CheckNumber(1), *static_cast<Nz::Vector2d*>(lua.CheckUserdata(1, "Vector2")));
					else if (lua.IsOfType(1, "Vector2"))
						return new Nz::Vector3d(*(*static_cast<Nz::Vector2d**>(lua.ToUserdata(1))), lua.CheckNumber(2));

					break;
				}
			}

			lua.Error("No matching overload for constructor");
			return nullptr;
		});

		vector3dClass.SetMethod("__tostring", &Nz::Vector3d::ToString);

		vector3dClass.SetGetter([] (Nz::LuaInstance& lua, Nz::Vector3d& instance)
		{
			switch (lua.GetType(1))
			{
				case Nz::LuaType_Number:
					lua.Push(instance[lua.CheckInteger(1)]);
					return true;

				case Nz::LuaType_String:
				{
					std::size_t length;
					const char* xyz = lua.CheckString(1, &length);

					if (length != 1)
						break;

					switch (xyz[0])
					{
						case 'x':
							lua.Push(instance.x);
							return true;

						case 'y':
							lua.Push(instance.y);
							return true;

						case 'z':
							lua.Push(instance.z);
							return true;
					}
					break;
				}
			}

			return false;
		});

		vector3dClass.SetSetter([] (Nz::LuaInstance& lua, Nz::Vector3d& instance)
		{
			switch (lua.GetType(1))
			{
				case Nz::LuaType_Number:
				{
					long long index = lua.CheckInteger(1);
					if (index < 1 || index > 3)
						return false;

					instance[index] = lua.CheckNumber(2);
					return true;
				}

				case Nz::LuaType_String:
				{
					std::size_t length;
					const char* xyz = lua.CheckString(1, &length);

					if (length != 1)
						break;

					double value = lua.CheckNumber(2);

					switch (xyz[0])
					{
						case 'x':
							instance.x = value;
							return true;

						case 'y':
							instance.y = value;
							return true;

						case 'z':
							instance.z = value;
							return true;
					}
					break;
				}
			}

			return false;
		});

		vector3dClass.Register(instance);
	}
}

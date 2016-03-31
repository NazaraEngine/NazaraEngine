// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/LuaAPI.hpp>
#include <Nazara/Math.hpp>
#include <Nazara/Lua/LuaClass.hpp>

namespace Ndk
{
	void LuaAPI::Register_Math(Nz::LuaInstance& instance)
	{
		/*********************************** Nz::EulerAngles **********************************/
		Nz::LuaClass<Nz::EulerAnglesd> eulerAnglesClass("EulerAngles");

		eulerAnglesClass.SetConstructor([] (Nz::LuaInstance& lua) -> Nz::EulerAnglesd*
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 3U);
			switch (argCount)
			{
				case 0:
					return new Nz::EulerAnglesd(0.0, 0.0, 0.0);

				case 1:
					return new Nz::EulerAnglesd(*(*static_cast<Nz::EulerAnglesd**>(lua.CheckUserdata(1, "EulerAngles"))));

				case 3:
					return new Nz::EulerAnglesd(lua.CheckNumber(1), lua.CheckNumber(2), lua.CheckNumber(3));
			}

			lua.Error("No matching overload for EulerAngles constructor");
			return nullptr;
		});

		eulerAnglesClass.SetMethod("__tostring", &Nz::EulerAnglesd::ToString);

		eulerAnglesClass.SetGetter([] (Nz::LuaInstance& lua, Nz::EulerAnglesd& instance)
		{
			std::size_t length;
			const char* ypr = lua.CheckString(1, &length);

			switch (length)
			{
				case 1:
				{
					switch (ypr[0])
					{
						case 'p':
							lua.Push(instance.pitch);
							return true;

						case 'y':
							lua.Push(instance.yaw);
							return true;

						case 'r':
							lua.Push(instance.roll);
							return true;
					}
					break;
				}

				case 3:
				{
					if (std::memcmp(ypr, "yaw", 3) != 0)
						break;

					lua.Push(instance.yaw);
					return true;
				}

				case 4:
				{
					if (std::memcmp(ypr, "roll", 4) != 0)
						break;

					lua.Push(instance.roll);
					return true;
				}

				case 5:
				{
					if (std::memcmp(ypr, "pitch", 5) != 0)
						break;

					lua.Push(instance.pitch);
					return true;
				}
			}

			return false;
		});

		eulerAnglesClass.SetSetter([] (Nz::LuaInstance& lua, Nz::EulerAnglesd& instance)
		{
			std::size_t length;
			const char* ypr = lua.CheckString(1, &length);
			double value = lua.CheckNumber(2);

			switch (length)
			{
				case 1:
				{
					switch (ypr[0])
					{
						case 'p':
							instance.pitch = value;
							return true;

						case 'y':
							instance.yaw = value;
							return true;

						case 'r':
							instance.roll = value;
							return true;
					}
					break;
				}

				case 3:
				{
					if (std::memcmp(ypr, "yaw", 3) != 0)
						break;

					instance.yaw = value;
					return true;
				}

				case 4:
				{
					if (std::memcmp(ypr, "roll", 4) != 0)
						break;

					instance.roll = value;
					return true;
				}

				case 5:
				{
					if (std::memcmp(ypr, "pitch", 5) != 0)
						break;

					instance.pitch = value;
					return true;
				}
			}

			return false;
		});

		eulerAnglesClass.Register(instance);

		/*********************************** Nz::Quaternion **********************************/
		Nz::LuaClass<Nz::Quaterniond> quaternionClass("Quaternion");

		quaternionClass.SetConstructor([] (Nz::LuaInstance& lua) -> Nz::Quaterniond*
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 4U);
			switch (argCount)
			{
				case 0:
					return new Nz::Quaterniond(1.0, 0.0, 0.0, 0.0);

				case 1:
				{
					if (lua.IsOfType(1, "EulerAngles"))
						return new Nz::Quaterniond(*(*static_cast<Nz::EulerAnglesd**>(lua.ToUserdata(1))));
					else if (lua.IsOfType(1, "Quaternion"))
						return new Nz::Quaterniond(*(*static_cast<Nz::Quaterniond**>(lua.ToUserdata(1))));
				}

				case 2:
					return new Nz::Quaterniond(lua.CheckNumber(1), *(*static_cast<Nz::Vector3d**>(lua.CheckUserdata(2, "Vector3"))));

				case 4:
					return new Nz::Quaterniond(lua.CheckNumber(1), lua.CheckNumber(2), lua.CheckNumber(3), lua.CheckNumber(4));
			}

			lua.Error("No matching overload for Quaternion constructor");
			return nullptr;
		});

		quaternionClass.SetMethod("__tostring", &Nz::Quaterniond::ToString);

		quaternionClass.SetGetter([] (Nz::LuaInstance& lua, Nz::Quaterniond& instance)
		{
			std::size_t length;
			const char* wxyz = lua.CheckString(1, &length);

			if (length != 1)
				return false;

			switch (wxyz[0])
			{
				case 'w':
					lua.Push(instance.w);
					return true;

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

			return false;
		});

		quaternionClass.SetSetter([] (Nz::LuaInstance& lua, Nz::Quaterniond& instance)
		{
			std::size_t length;
			const char* wxyz = lua.CheckString(1, &length);

			if (length != 1)
				return false;

			double value = lua.CheckNumber(2);

			switch (wxyz[0])
			{
				case 'w':
					instance.w = value;
					return true;

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

			return false;
		});

		quaternionClass.Register(instance);

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
						return new Nz::Vector2d(*(*static_cast<Nz::Vector2d**>(lua.ToUserdata(1))));

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
				{
					long long index = lua.CheckInteger(1);
					if (index < 1 || index > 2)
						return false;

					lua.Push(instance[index - 1]);
					return true;
				}

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

					instance[index - 1] = lua.CheckNumber(2);
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
						return new Nz::Vector3d(lua.CheckNumber(1), *(*static_cast<Nz::Vector2d**>(lua.ToUserdata(1))));
					else if (lua.IsOfType(1, "Vector2"))
						return new Nz::Vector3d(*(*static_cast<Nz::Vector2d**>(lua.ToUserdata(1))));
					else if (lua.IsOfType(1, "Vector3"))
						return new Nz::Vector3d(*(*static_cast<Nz::Vector3d**>(lua.ToUserdata(1))));

					break;
				}

				case 2:
				{
					if (lua.IsOfType(1, Nz::LuaType_Number))
						return new Nz::Vector3d(lua.CheckNumber(1), *(*static_cast<Nz::Vector2d**>(lua.CheckUserdata(1, "Vector2"))));
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
				{
					long long index = lua.CheckInteger(1);
					if (index < 1 || index > 3)
						return false;

					lua.Push(instance[index - 1]);
					return true;
				}

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

					instance[index - 1] = lua.CheckNumber(2);
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

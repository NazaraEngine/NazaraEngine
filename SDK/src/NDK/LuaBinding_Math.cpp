// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/LuaBinding.hpp>
#include <NDK/LuaAPI.hpp>
#include <cstring>

#include <iostream>

namespace Ndk
{
	void LuaBinding::BindMath()
	{
		/*********************************** Nz::EulerAngles **********************************/
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

		/*********************************** Nz::Quaternion **********************************/
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

        /*********************************** Nz::Rect **********************************/
        rectClass.SetConstructor([](Nz::LuaInstance& lua) -> Nz::Rectd*
        {
            unsigned int argCount = std::min(lua.GetStackTop(), 4U);
            switch (argCount) {
                case 0:
                case 4:
                    return new Nz::Rectd(lua.CheckNumber(1, 0.0), lua.CheckNumber(2, 0.0), lua.CheckNumber(3, 0.0), lua.CheckNumber(4, 0.0));

                case 1:
                {
                    if (lua.IsOfType(1, "Rect"))
                        return new Nz::Rectd(*(*static_cast<Nz::Rectd**>(lua.ToUserdata(1))));
                    else if (lua.IsOfType(1, Nz::LuaType_Table))
                    {
                        // TODO => Faire sans avoir à mettre de nom dans la table et prendre les éléments un à un pour créer le Rectd
                        return new Nz::Rectd(lua.CheckField<double>("x", 1),
                                             lua.CheckField<double>("y", 1),
                                             lua.CheckField<double>("width", 1),
                                             lua.CheckField<double>("height", 1));
                    }
                    else if (lua.IsOfType(1, "Vector"))
                        return new Nz::Rectd(*(*static_cast<Nz::Vector2d**>(lua.ToUserdata(1))));

                    break;
                }

                case 2:
                {
                    if (lua.IsOfType(1, Nz::LuaType_Number) && lua.IsOfType(2, Nz::LuaType_Number))
                        return new Nz::Rectd(lua.CheckNumber(1), lua.CheckNumber(2));
                    else if (lua.IsOfType(1, "Vector2") && lua.IsOfType(2, "Vector2"))
                        return new Nz::Rectd(*(*static_cast<Nz::Vector2d**>(lua.ToUserdata(1))), *(*static_cast<Nz::Vector2d**>(lua.ToUserdata(2))));

                    break;
                }
            }

            lua.Error("No matching overload for Rect constructor");
            return nullptr;
        });

        rectClass.SetMethod("__tostring", &Nz::Rectd::ToString);

        rectClass.SetGetter([](Nz::LuaInstance& lua, Nz::Rectd& instance)
        {
            switch (lua.GetType(1))
            {
                case Nz::LuaType_Number:
                    lua.Push(instance[lua.CheckInteger(1)]);
                    return true;

                case Nz::LuaType_String:
                {
                    std::size_t lenght;
                    const char* xywh = lua.CheckString(1, &lenght);
                    if (lenght != 1 && lenght != 5 && lenght != 6)
                        break;

                    if (xywh == "x")
                    {
                        lua.Push(instance.x);
                        return true;
                    }
                    else if(xywh == "y")
                    {
                        lua.Push(instance.y);
                        return true;
                    }
                    else if (xywh == "width")
                    {
                        lua.Push(instance.width);
                        return true;
                    }
                    else if (xywh == "height")
                    {
                        lua.Push(instance.height);
                        return true;
                    }
                    break;
                }
            }

            return false;
        });

        rectClass.SetSetter([](Nz::LuaInstance& lua, Nz::Rectd& instance)
        {
            switch (lua.GetType(1))
            {
                case Nz::LuaType_Number:
                {
                    long long index = lua.CheckInteger(1);
                    if (index < 1 || index > 4)
                        return false;

                    instance[index] = lua.CheckNumber(2);
                    return true;
                }

                case Nz::LuaType_String:
                {
                    std::size_t lenght;
                    const char* xywh = lua.CheckString(1, &lenght);

                    if (lenght != 1 && lenght != 5 && lenght != 6)
                        break;

                    double value = lua.CheckNumber(2);

                    if (xywh == "x")
                    {
                        instance.x = value;
                        return true;
                    }
                    else if(xywh == "y")
                    {
                        instance.y = value;
                        return true;
                    }
                    else if (xywh == "width")
                    {
                        instance.width = value;
                        return true;
                    }
                    else if (xywh == "height")
                    {
                        instance.height = value;
                        return true;
                    }
                    break;
                }
            }

            return false;
        });

		/*********************************** Nz::Vector2 **********************************/
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

		/*********************************** Nz::Vector3 **********************************/
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
	}

	void LuaBinding::RegisterMath(Nz::LuaInstance& instance)
	{
		eulerAnglesClass.Register(instance);
		quaternionClass.Register(instance);
        rectClass.Register(instance);
		vector2dClass.Register(instance);
		vector3dClass.Register(instance);
	}
}

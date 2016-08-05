// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/LuaBinding.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <NDK/LuaAPI.hpp>
#include <cstring>

namespace Ndk
{
	void LuaBinding::BindMath()
	{
		/*********************************** Nz::EulerAngles **********************************/
		eulerAnglesClass.SetConstructor([] (Nz::LuaInstance& lua, Nz::EulerAnglesd* angles)
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 3U);
			switch (argCount)
			{
				case 0:
					Nz::PlacementNew(angles, Nz::EulerAnglesd::Zero());
					return true;

				case 1:
					Nz::PlacementNew(angles, *static_cast<Nz::EulerAnglesd*>(lua.CheckUserdata(1, "EulerAngles")));
					return true;

				case 3:
					Nz::PlacementNew(angles, lua.CheckNumber(1), lua.CheckNumber(2), lua.CheckNumber(3));
					return true;
			}

			lua.Error("No matching overload for EulerAngles constructor");
			return false;
		});

		eulerAnglesClass.BindMethod("__tostring", &Nz::EulerAnglesd::ToString);

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

		/*********************************** Nz::Rect **********************************/
		rectClass.SetConstructor([] (Nz::LuaInstance& lua, Nz::Rectd* rect)
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 4U);
			switch (argCount)
			{
				case 0:
				case 4:
					PlacementNew(rect, lua.CheckNumber(1, 0.0), lua.CheckNumber(2, 0.0), lua.CheckNumber(3, 0.0), lua.CheckNumber(4, 0.0));
					return true;

				case 1:
				{
					if (lua.IsOfType(1, "Rect"))
						PlacementNew(rect, *static_cast<Nz::Rectd*>(lua.ToUserdata(1)));
					else if (lua.IsOfType(1, Nz::LuaType_Table))
					{
						// TODO => Faire sans avoir à mettre de nom dans la table et prendre les éléments un à un pour créer le Rectd
						PlacementNew(rect, lua.CheckField<double>("x", 1),
						                   lua.CheckField<double>("y", 1),
						                   lua.CheckField<double>("width", 1),
						                   lua.CheckField<double>("height", 1));
					}
					else if (lua.IsOfType(1, "Vector2"))
						PlacementNew(rect, *static_cast<Nz::Vector2d*>(lua.ToUserdata(1)));
					else
						break;

					return true;
				}

				case 2:
				{
					if (lua.IsOfType(1, Nz::LuaType_Number) && lua.IsOfType(2, Nz::LuaType_Number))
						PlacementNew(rect, lua.CheckNumber(1), lua.CheckNumber(2));
					else if (lua.IsOfType(1, "Vector2") && lua.IsOfType(2, "Vector2"))
						PlacementNew(rect, *static_cast<Nz::Vector2d*>(lua.ToUserdata(1)), *static_cast<Nz::Vector2d*>(lua.ToUserdata(2)));
					else
						break;

					return true;
				}
			}

			lua.Error("No matching overload for Rect constructor");
			return false;
		});

		rectClass.BindMethod("__tostring", &Nz::Rectd::ToString);

		rectClass.SetGetter([] (Nz::LuaInstance& lua, Nz::Rectd& instance)
		{
			switch (lua.GetType(1))
			{
				case Nz::LuaType_Number:
				{
					auto index = lua.CheckBoundInteger<std::size_t>(1);
					if (index < 1 || index > 4)
						return false;

					lua.Push(instance[index - 1]);
					return true;
				}

				case Nz::LuaType_String:
				{
					std::size_t length;
					const char* xywh = lua.CheckString(1, &length);

					if (length != 1)
						break;

					switch (xywh[0])
					{
						case 'x':
							lua.Push(instance.x);
							return true;

						case 'y':
							lua.Push(instance.y);
							return true;

						case 'w':
							lua.Push(instance.width);
							return true;

						case 'h':
							lua.Push(instance.height);
							return true;

						default:
							break;
					}
					break;
				}

				default:
					break;
			}

			return false;
		});

		rectClass.SetSetter([] (Nz::LuaInstance& lua, Nz::Rectd& instance)
		{
			switch (lua.GetType(1))
			{
				case Nz::LuaType_Number:
				{
					auto index = lua.CheckBoundInteger<std::size_t>(1);
					if (index < 1 || index > 4)
						return false;

					instance[index - 1] = lua.CheckNumber(2);
					return true;
				}

				case Nz::LuaType_String:
				{
					std::size_t length;
					const char* xywh = lua.CheckString(1, &length);

					if (length != 1)
						break;

					double value = lua.CheckNumber(2);

					switch (xywh[0])
					{
						case 'x':
							instance.x = value;
							return true;

						case 'y':
							instance.y = value;
							return true;

						case 'w':
							instance.width = value;
							return true;

						case 'h':
							instance.height = value;
							return true;
					}
					break;
				}

				default:
					break;
			}

			return false;
		});

		/*********************************** Nz::Quaternion **********************************/
		quaternionClass.SetConstructor([] (Nz::LuaInstance& lua, Nz::Quaterniond* quaternion)
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 4U);
			switch (argCount)
			{
				case 0:
					Nz::PlacementNew(quaternion, Nz::Quaterniond::Zero());
					return true;

				case 1:
				{
					if (lua.IsOfType(1, "EulerAngles"))
						Nz::PlacementNew(quaternion, *static_cast<Nz::EulerAnglesd*>(lua.ToUserdata(1)));
					else if (lua.IsOfType(1, "Quaternion"))
						Nz::PlacementNew(quaternion, *static_cast<Nz::Quaterniond*>(lua.ToUserdata(1)));
					else
						break;

					return true;
				}

				case 2:
					Nz::PlacementNew(quaternion, lua.CheckNumber(1), *static_cast<Nz::Vector3d*>(lua.CheckUserdata(2, "Vector3")));
					return true;

				case 4:
					Nz::PlacementNew(quaternion, lua.CheckNumber(1), lua.CheckNumber(2), lua.CheckNumber(3), lua.CheckNumber(4));
					return true;
			}

			lua.Error("No matching overload for Quaternion constructor");
			return false;
		});

		quaternionClass.BindMethod("__tostring", &Nz::Quaterniond::ToString);

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

				default:
					break;
			}

			return false;
		});

		/*********************************** Nz::Vector2 **********************************/
		vector2dClass.SetConstructor([](Nz::LuaInstance& lua, Nz::Vector2d* vector)
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 2U);
			switch (argCount)
			{
				case 0:
				case 2:
					Nz::PlacementNew(vector, lua.CheckNumber(1, 0.0), lua.CheckNumber(2, 0.0));
					return true;

				case 1:
				{
					if (lua.IsOfType(1, Nz::LuaType_Number))
						Nz::PlacementNew(vector, lua.CheckNumber(1));
					else if (lua.IsOfType(1, "Vector2"))
						Nz::PlacementNew(vector, *static_cast<Nz::Vector2d*>(lua.ToUserdata(1)));
					else
						break;

					return true;
				}
			}

			lua.Error("No matching overload for Vector2 constructor");
			return false;
		});

		vector2dClass.BindMethod("__tostring", &Nz::Vector2d::ToString);

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

						default:
							break;
					}
					break;
				}

				default:
					break;
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

						default:
							break;
					}
					break;
				}

				default:
					break;
			}

			return false;
		});

		/*********************************** Nz::Vector3 **********************************/
		vector3dClass.SetConstructor([] (Nz::LuaInstance& lua, Nz::Vector3d* vector)
		{
			unsigned int argCount = std::min(lua.GetStackTop(), 3U);
			switch (argCount)
			{
				case 0:
				case 3:
					Nz::PlacementNew(vector, lua.CheckNumber(1, 0.0), lua.CheckNumber(2, 0.0), lua.CheckNumber(3, 0.0));
					return true;

				case 1:
				{
					if (lua.IsOfType(1, Nz::LuaType_Number))
						Nz::PlacementNew(vector, lua.CheckNumber(1), *static_cast<Nz::Vector2d*>(lua.CheckUserdata(1, "Vector2")));
					else if (lua.IsOfType(1, "Vector2"))
						Nz::PlacementNew(vector, *static_cast<Nz::Vector2d*>(lua.ToUserdata(1)));
					else if (lua.IsOfType(1, "Vector3"))
						Nz::PlacementNew(vector, *static_cast<Nz::Vector3d*>(lua.ToUserdata(1)));
					else
						break;

					return true;
				}

				case 2:
				{
					if (lua.IsOfType(1, Nz::LuaType_Number))
						Nz::PlacementNew(vector, lua.CheckNumber(1), *static_cast<Nz::Vector2d*>(lua.CheckUserdata(1, "Vector2")));
					else if (lua.IsOfType(1, "Vector2"))
						Nz::PlacementNew(vector, *static_cast<Nz::Vector2d*>(lua.ToUserdata(1)), lua.CheckNumber(2));
					else
						break;

					return true;
				}
			}

			lua.Error("No matching overload for constructor");
			return false;
		});

		vector3dClass.BindMethod("__tostring", &Nz::Vector3d::ToString);

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

						default:
							break;
					}
					break;
				}

				default:
					break;
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

						default:
							break;
					}
					break;
				}

				default:
					break;
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

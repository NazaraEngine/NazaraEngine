// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/Lua/LuaBinding_Math.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <NDK/LuaAPI.hpp>
#include <cstring>

namespace Ndk
{
	std::unique_ptr<LuaBinding_Base> LuaBinding_Base::BindMath(LuaBinding& binding)
	{
		return std::make_unique<LuaBinding_Math>(binding);
	}

	LuaBinding_Math::LuaBinding_Math(LuaBinding& binding) :
	LuaBinding_Base(binding)
	{
		/*********************************** Nz::EulerAngles **********************************/
		eulerAngles.Reset("EulerAngles");
		{
			eulerAngles.SetConstructor([] (Nz::LuaInstance& lua, Nz::EulerAnglesd* instance, std::size_t argumentCount)
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 1U);

				switch (argCount)
				{
					case 0:
						Nz::PlacementNew(instance, Nz::EulerAnglesd::Zero());
						return true;

					case 1:
						Nz::PlacementNew(instance, *static_cast<Nz::EulerAnglesd*>(lua.CheckUserdata(1, "EulerAngles")));
						return true;

					case 3:
						Nz::PlacementNew(instance, lua.CheckNumber(1), lua.CheckNumber(2), lua.CheckNumber(3));
						return true;
				}

				lua.Error("No matching overload for EulerAngles constructor");
				return false;
			});

			eulerAngles.BindMethod("Normalize", &Nz::EulerAnglesd::Normalize);
			eulerAngles.BindMethod("ToQuaternion", &Nz::EulerAnglesd::ToQuaternion);

			eulerAngles.BindMethod("__tostring", &Nz::EulerAnglesd::ToString);

			eulerAngles.SetGetter([] (Nz::LuaInstance& lua, Nz::EulerAnglesd& instance)
			{
				std::size_t length;
				const char* ypr = lua.CheckString(2, &length);

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

			eulerAngles.SetSetter([] (Nz::LuaInstance& lua, Nz::EulerAnglesd& instance)
			{
				std::size_t length;
				const char* ypr = lua.CheckString(2, &length);
				double value = lua.CheckNumber(3);

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
		}

		/*********************************** Nz::Matrix4 **********************************/
		matrix4d.Reset("Matrix4");
		{
			matrix4d.SetConstructor([] (Nz::LuaInstance& lua, Nz::Matrix4d* matrix, std::size_t argumentCount)
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 3U);

				switch (argCount)
				{
					case 0:
						Nz::PlacementNew(matrix, Nz::Matrix4d::Zero());
						return true;

					case 1:
						if (lua.IsOfType(1, "Matrix4"))
							Nz::PlacementNew(matrix, *static_cast<Nz::Matrix4d*>(lua.ToUserdata(1)));
						break;

					case 16:
					{
						double values[16];
						for (int i = 0; i < 16; ++i)
							values[i] = lua.CheckNumber(i);

						Nz::PlacementNew(matrix, values);

						return true;
					}
				}

				lua.Error("No matching overload for constructor");
				return false;
			});

			matrix4d.BindMethod("ApplyRotation", &Nz::Matrix4d::ApplyRotation);
			matrix4d.BindMethod("ApplyScale", &Nz::Matrix4d::ApplyScale);
			matrix4d.BindMethod("ApplyTranslation", &Nz::Matrix4d::ApplyTranslation);

			matrix4d.BindMethod("Concatenate", &Nz::Matrix4d::Concatenate);
			matrix4d.BindMethod("ConcatenateAffine", &Nz::Matrix4d::ConcatenateAffine);

			//matrix4d.BindMethod("GetColumn",            &Nz::Matrix4d::GetColumn);
			matrix4d.BindMethod("GetDeterminant", &Nz::Matrix4d::GetDeterminant);
			matrix4d.BindMethod("GetDeterminantAffine", &Nz::Matrix4d::GetDeterminantAffine);

			matrix4d.BindMethod("GetInverse", [] (Nz::LuaInstance& lua, Nz::Matrix4d& instance, std::size_t /*argumentCount*/) -> int
			{
				Nz::Matrix4d result;
				if (instance.GetInverse(&result))
					return lua.Push(true, result);
				else
					return lua.Push(false);
			});

			matrix4d.BindMethod("GetInverseAffine", [] (Nz::LuaInstance& lua, Nz::Matrix4d& instance, std::size_t /*argumentCount*/) -> int
			{
				Nz::Matrix4d result;
				if (instance.GetInverseAffine(&result))
					return lua.Push(true, result);
				else
					return lua.Push(false);
			});

			matrix4d.BindMethod("GetRotation", &Nz::Matrix4d::GetRotation);

			//matrix4d.BindMethod("GetRow", &Nz::Matrix4d::GetRow);
			matrix4d.BindMethod("GetScale", &Nz::Matrix4d::GetScale);
			matrix4d.BindMethod("GetSquaredScale", &Nz::Matrix4d::GetSquaredScale);
			matrix4d.BindMethod("GetTranslation", &Nz::Matrix4d::GetTranslation);

			matrix4d.BindMethod("GetTransposed", [] (Nz::LuaInstance& lua, Nz::Matrix4d& instance, std::size_t /*argumentCount*/) -> int
			{
				Nz::Matrix4d result;
				instance.GetTransposed(&result);

				return lua.Push(result);
			});

			matrix4d.BindMethod("HasNegativeScale", &Nz::Matrix4d::HasNegativeScale);
			matrix4d.BindMethod("HasScale", &Nz::Matrix4d::HasScale);

			matrix4d.BindMethod("Inverse", [] (Nz::LuaInstance& lua, Nz::Matrix4d& instance, std::size_t /*argumentCount*/) -> int
			{
				bool succeeded;
				instance.Inverse(&succeeded);

				return lua.Push(succeeded);
			});

			matrix4d.BindMethod("InverseAffine", [] (Nz::LuaInstance& lua, Nz::Matrix4d& instance, std::size_t /*argumentCount*/) -> int
			{
				bool succeeded;
				instance.InverseAffine(&succeeded);

				return lua.Push(succeeded);
			});

			matrix4d.BindMethod("IsAffine", &Nz::Matrix4d::IsAffine);
			matrix4d.BindMethod("IsIdentity", &Nz::Matrix4d::IsIdentity);

			matrix4d.BindMethod("MakeIdentity", &Nz::Matrix4d::MakeIdentity);
			matrix4d.BindMethod("MakeLookAt", &Nz::Matrix4d::MakeLookAt, Nz::Vector3d::Up());
			matrix4d.BindMethod("MakeOrtho", &Nz::Matrix4d::MakeOrtho, -1.0, 1.0);
			matrix4d.BindMethod("MakePerspective", &Nz::Matrix4d::MakePerspective);
			matrix4d.BindMethod("MakeRotation", &Nz::Matrix4d::MakeRotation);
			matrix4d.BindMethod("MakeScale", &Nz::Matrix4d::MakeScale);
			matrix4d.BindMethod("MakeTranslation", &Nz::Matrix4d::MakeTranslation);
			matrix4d.BindMethod("MakeTransform", (Nz::Matrix4d&(Nz::Matrix4d::*)(const Nz::Vector3d&, const Nz::Quaterniond&, const Nz::Vector3d&)) &Nz::Matrix4d::MakeTransform, Nz::Vector3d::Unit());
			matrix4d.BindMethod("MakeViewMatrix", &Nz::Matrix4d::MakeViewMatrix);
			matrix4d.BindMethod("MakeZero", &Nz::Matrix4d::MakeZero);

			matrix4d.BindMethod("Set", [] (Nz::LuaInstance& lua, Nz::Matrix4d& instance, std::size_t argumentCount) -> int
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 3U);

				int argIndex = 2;
				switch (argCount)
				{
					case 1:
						if (lua.IsOfType(argIndex, "Matrix4"))
							instance.Set(*static_cast<Nz::Matrix4d*>(lua.ToUserdata(argIndex)));
						break;

					case 16:
					{
						double values[16];
						for (std::size_t i = 0; i < 16; ++i)
							values[i] = lua.CheckNumber(argIndex++);

						instance.Set(values);

						return 0;
					}
				}

				lua.Error("No matching overload for method Set");
				return 0;
			});

			matrix4d.BindMethod("SetRotation", &Nz::Matrix4d::SetRotation);
			matrix4d.BindMethod("SetScale", &Nz::Matrix4d::SetScale);
			matrix4d.BindMethod("SetTranslation", &Nz::Matrix4d::SetTranslation);

			matrix4d.BindMethod("Transform", [] (Nz::LuaInstance& lua, Nz::Matrix4d& instance, std::size_t /*argumentCount*/) -> int
			{
				int argIndex = 2;
				if (lua.IsOfType(argIndex, "Vector2"))
				{
					double z(lua.CheckNumber(argIndex + 1, 0.0));
					double w(lua.CheckNumber(argIndex + 2, 1.0));

					return lua.Push(instance.Transform(*static_cast<Nz::Vector2d*>(lua.ToUserdata(argIndex)), z, w));
				}
				else if (lua.IsOfType(argIndex, "Vector3"))
				{
					double w(lua.CheckNumber(argIndex + 1, 1.0));

					return lua.Push(instance.Transform(*static_cast<Nz::Vector3d*>(lua.ToUserdata(argIndex)), w));
				}
				//else if (lua.IsOfType(2, "Vector4"))
				//	return lua.Push(instance.Transform(*static_cast<Nz::Vector4d*>(lua.ToUserdata(1))));

				lua.Error("No matching overload for method Transform");
				return 0;
			});

			matrix4d.BindMethod("Transpose", &Nz::Matrix4d::Transpose);

			matrix4d.BindMethod("__tostring", &Nz::Matrix4d::ToString);

			matrix4d.BindStaticMethod("Concatenate", &Nz::Matrix4d::Concatenate);
			matrix4d.BindStaticMethod("ConcatenateAffine", &Nz::Matrix4d::ConcatenateAffine);
			matrix4d.BindStaticMethod("Identity", &Nz::Matrix4d::Identity);
			matrix4d.BindStaticMethod("LookAt", &Nz::Matrix4d::LookAt, Nz::Vector3d::Up());
			matrix4d.BindStaticMethod("Ortho", &Nz::Matrix4d::Ortho, -1.0, 1.0);
			matrix4d.BindStaticMethod("Perspective", &Nz::Matrix4d::Perspective);
			matrix4d.BindStaticMethod("Rotate", &Nz::Matrix4d::Rotate);
			matrix4d.BindStaticMethod("Scale", &Nz::Matrix4d::Scale);
			matrix4d.BindStaticMethod("Translate", &Nz::Matrix4d::Translate);
			matrix4d.BindStaticMethod("Transform", (Nz::Matrix4d(*)(const Nz::Vector3d&, const Nz::Quaterniond&, const Nz::Vector3d&)) &Nz::Matrix4d::Transform, Nz::Vector3d::Unit());
			matrix4d.BindStaticMethod("ViewMatrix", &Nz::Matrix4d::ViewMatrix);
			matrix4d.BindStaticMethod("Zero", &Nz::Matrix4d::Zero);

			matrix4d.SetGetter([] (Nz::LuaInstance& lua, Nz::Matrix4d& instance)
			{
				bool succeeded = false;
				std::size_t index = static_cast<std::size_t>(lua.ToInteger(2, &succeeded));
				if (!succeeded || index < 1 || index > 16)
					return false;

				lua.Push(instance[index - 1]);
				return true;
			});

			matrix4d.SetSetter([] (Nz::LuaInstance& lua, Nz::Matrix4d& instance)
			{
				bool succeeded = false;
				std::size_t index = static_cast<std::size_t>(lua.ToInteger(2, &succeeded));
				if (!succeeded || index < 1 || index > 16)
					return false;

				instance[index - 1] = lua.CheckNumber(3);

				return true;
			});
		}

		/*********************************** Nz::Rect **********************************/
		rect.Reset("Rect");
		{
			rect.SetConstructor([] (Nz::LuaInstance& lua, Nz::Rectd* instance, std::size_t argumentCount)
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 4U);

				switch (argCount)
				{
					case 0:
					case 4:
						PlacementNew(instance, lua.CheckNumber(1, 0.0), lua.CheckNumber(2, 0.0), lua.CheckNumber(3, 0.0), lua.CheckNumber(4, 0.0));
						return true;

					case 1:
					{
						if (lua.IsOfType(1, "Rect"))
							PlacementNew(instance, *static_cast<Nz::Rectd*>(lua.ToUserdata(1)));
						else if (lua.IsOfType(1, Nz::LuaType_Table))
						{
							// TODO => Faire sans avoir à mettre de nom dans la table et prendre les éléments un à un pour créer le Rectd
							PlacementNew(instance, lua.CheckField<double>("x", 1),
										 lua.CheckField<double>("y", 1),
										 lua.CheckField<double>("width", 1),
										 lua.CheckField<double>("height", 1));
						}
						else if (lua.IsOfType(1, "Vector2"))
							PlacementNew(instance, *static_cast<Nz::Vector2d*>(lua.ToUserdata(1)));
						else
							break;

						return true;
					}

					case 2:
					{
						if (lua.IsOfType(1, Nz::LuaType_Number) && lua.IsOfType(2, Nz::LuaType_Number))
							PlacementNew(instance, lua.CheckNumber(1), lua.CheckNumber(2));
						else if (lua.IsOfType(1, "Vector2") && lua.IsOfType(2, "Vector2"))
							PlacementNew(instance, *static_cast<Nz::Vector2d*>(lua.ToUserdata(1)), *static_cast<Nz::Vector2d*>(lua.ToUserdata(2)));
						else
							break;

						return true;
					}
				}

				lua.Error("No matching overload for Rect constructor");
				return false;
			});

			rect.BindMethod("__tostring", &Nz::Rectd::ToString);

			rect.SetGetter([] (Nz::LuaInstance& lua, Nz::Rectd& instance)
			{
				switch (lua.GetType(2))
				{
					case Nz::LuaType_Number:
					{
						auto index = lua.CheckBoundInteger<std::size_t>(2);
						if (index < 1 || index > 4)
							return false;

						lua.Push(instance[index - 1]);
						return true;
					}

					case Nz::LuaType_String:
					{
						std::size_t length;
						const char* xywh = lua.CheckString(2, &length);

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

			rect.SetSetter([] (Nz::LuaInstance& lua, Nz::Rectd& instance)
			{
				switch (lua.GetType(2))
				{
					case Nz::LuaType_Number:
					{
						auto index = lua.CheckBoundInteger<std::size_t>(2);
						if (index < 1 || index > 4)
							return false;

						instance[index - 1] = lua.CheckNumber(2);
						return true;
					}

					case Nz::LuaType_String:
					{
						std::size_t length;
						const char* xywh = lua.CheckString(2, &length);

						if (length != 1)
							break;

						double value = lua.CheckNumber(3);

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
		}

		/*********************************** Nz::Quaternion **********************************/
		quaternion.Reset("Quaternion");
		{
			quaternion.SetConstructor([] (Nz::LuaInstance& lua, Nz::Quaterniond* instance, std::size_t argumentCount)
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 4U);

				switch (argCount)
				{
					case 0:
						Nz::PlacementNew(instance, Nz::Quaterniond::Zero());
						return true;

					case 1:
					{
						if (lua.IsOfType(1, "EulerAngles"))
							Nz::PlacementNew(instance, *static_cast<Nz::EulerAnglesd*>(lua.ToUserdata(1)));
						else if (lua.IsOfType(1, "Quaternion"))
							Nz::PlacementNew(instance, *static_cast<Nz::Quaterniond*>(lua.ToUserdata(1)));
						else
							break;

						return true;
					}

					case 2:
						Nz::PlacementNew(instance, lua.CheckNumber(1), *static_cast<Nz::Vector3d*>(lua.CheckUserdata(2, "Vector3")));
						return true;

					case 4:
						Nz::PlacementNew(instance, lua.CheckNumber(1), lua.CheckNumber(2), lua.CheckNumber(3), lua.CheckNumber(4));
						return true;

					default:
						break;
				}

				lua.Error("No matching overload for Quaternion constructor");
				return false;
			});

			quaternion.BindMethod("ComputeW", &Nz::Quaterniond::ComputeW);
			quaternion.BindMethod("Conjugate", &Nz::Quaterniond::Conjugate);
			quaternion.BindMethod("DotProduct", &Nz::Quaterniond::DotProduct);
			quaternion.BindMethod("GetConjugate", &Nz::Quaterniond::GetConjugate);
			quaternion.BindMethod("GetInverse", &Nz::Quaterniond::GetInverse);

			quaternion.BindMethod("Inverse", &Nz::Quaterniond::Inverse);
			quaternion.BindMethod("Magnitude", &Nz::Quaterniond::Magnitude);

			quaternion.BindMethod("SquaredMagnitude", &Nz::Quaterniond::SquaredMagnitude);
			quaternion.BindMethod("ToEulerAngles", &Nz::Quaterniond::ToEulerAngles);

			quaternion.BindMethod("__tostring", &Nz::Quaterniond::ToString);

			quaternion.BindStaticMethod("Lerp", &Nz::Quaterniond::Lerp);
			quaternion.BindStaticMethod("RotationBetween", &Nz::Quaterniond::RotationBetween);
			quaternion.BindStaticMethod("Slerp", &Nz::Quaterniond::Slerp);

			quaternion.BindMethod("GetNormal", [] (Nz::LuaInstance& lua, Nz::Quaterniond& instance, std::size_t /*argumentCount*/) -> int
			{
				double length;

				lua.Push(instance.GetNormal(&length));
				lua.Push(length);

				return 2;
			});

			quaternion.BindMethod("Normalize", [] (Nz::LuaInstance& lua, Nz::Quaterniond& instance, std::size_t /*argumentCount*/) -> int
			{
				double length;

				instance.Normalize(&length);
				lua.Push(1); //< instance
				lua.Push(length);

				return 2;
			});

			quaternion.BindStaticMethod("Normalize", [] (Nz::LuaInstance& instance) -> int
			{
				int argIndex = 1;
				Nz::Quaterniond quat = instance.Check<Nz::Quaterniond>(&argIndex);

				double length;

				instance.Push(Nz::Quaterniond::Normalize(quat, &length));
				instance.Push(length);

				return 2;
			});

			quaternion.SetGetter([] (Nz::LuaInstance& lua, Nz::Quaterniond& instance)
			{
				std::size_t length;
				const char* wxyz = lua.CheckString(2, &length);

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

			quaternion.SetSetter([] (Nz::LuaInstance& lua, Nz::Quaterniond& instance)
			{
				std::size_t length;
				const char* wxyz = lua.CheckString(2, &length);

				if (length != 1)
					return false;

				double value = lua.CheckNumber(3);

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
		}

		/*********************************** Nz::Vector2 **********************************/
		vector2d.Reset("Vector2");
		{
			vector2d.SetConstructor([] (Nz::LuaInstance& lua, Nz::Vector2d* vector, std::size_t argumentCount)
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 2U);

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

			vector2d.BindMethod("__tostring", &Nz::Vector2d::ToString);

			vector2d.SetGetter([] (Nz::LuaInstance& lua, Nz::Vector2d& instance)
			{
				switch (lua.GetType(2))
				{
					case Nz::LuaType_Number:
					{
						long long index = lua.CheckInteger(2);
						if (index < 1 || index > 2)
							return false;

						lua.Push(instance[index - 1]);
						return true;
					}

					case Nz::LuaType_String:
					{
						std::size_t length;
						const char* xy = lua.CheckString(2, &length);

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

			vector2d.SetSetter([] (Nz::LuaInstance& lua, Nz::Vector2d& instance)
			{
				switch (lua.GetType(2))
				{
					case Nz::LuaType_Number:
					{
						long long index = lua.CheckInteger(2);
						if (index < 1 || index > 2)
							return false;

						instance[index - 1] = lua.CheckNumber(3);
						return true;
					}

					case Nz::LuaType_String:
					{
						std::size_t length;
						const char* xy = lua.CheckString(2, &length);

						if (length != 1)
							break;

						double value = lua.CheckNumber(3);

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
		}

		/*********************************** Nz::Vector3 **********************************/
		vector3d.Reset("Vector3");
		{
			vector3d.SetConstructor([] (Nz::LuaInstance& lua, Nz::Vector3d* vector, std::size_t argumentCount)
			{
				std::size_t argCount = std::min<std::size_t>(argumentCount, 3U);

				switch (argCount)
				{
					case 0:
					case 3:
						Nz::PlacementNew(vector, lua.CheckNumber(1, 0.0), lua.CheckNumber(2, 0.0), lua.CheckNumber(3, 0.0));
						return true;

					case 1:
					{
						if (lua.IsOfType(1, Nz::LuaType_Number))
							Nz::PlacementNew(vector, lua.CheckNumber(1));
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
							Nz::PlacementNew(vector, lua.CheckNumber(1), *static_cast<Nz::Vector2d*>(lua.CheckUserdata(2, "Vector2")));
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

			vector3d.BindMethod("__tostring", &Nz::Vector3d::ToString);

			vector3d.SetGetter([] (Nz::LuaInstance& lua, Nz::Vector3d& instance)
			{
				switch (lua.GetType(2))
				{
					case Nz::LuaType_Number:
					{
						long long index = lua.CheckInteger(2);
						if (index < 1 || index > 3)
							return false;

						lua.Push(instance[index - 1]);
						return true;
					}

					case Nz::LuaType_String:
					{
						std::size_t length;
						const char* xyz = lua.CheckString(2, &length);

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

			vector3d.SetSetter([] (Nz::LuaInstance& lua, Nz::Vector3d& instance)
			{
				switch (lua.GetType(2))
				{
					case Nz::LuaType_Number:
					{
						long long index = lua.CheckInteger(2);
						if (index < 1 || index > 3)
							return false;

						instance[index - 1] = lua.CheckNumber(3);
						return true;
					}

					case Nz::LuaType_String:
					{
						std::size_t length;
						const char* xyz = lua.CheckString(2, &length);

						if (length != 1)
							break;

						double value = lua.CheckNumber(3);

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
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the Math classes
	*/
	void LuaBinding_Math::Register(Nz::LuaInstance& instance)
	{
		eulerAngles.Register(instance);
		matrix4d.Register(instance);
		quaternion.Register(instance);
		rect.Register(instance);
		vector2d.Register(instance);
		vector3d.Register(instance);

		quaternion.PushGlobalTable(instance);
		{
			instance.PushField("Identity", Nz::Quaterniond::Identity());
			instance.PushField("Zero", Nz::Quaterniond::Zero());
		}
		instance.Pop();
	}
}

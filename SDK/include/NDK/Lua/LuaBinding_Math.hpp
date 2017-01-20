// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_LUABINDING_MATH_HPP
#define NDK_LUABINDING_MATH_HPP

#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NDK/Lua/LuaBinding_Base.hpp>

namespace Ndk
{
	class NDK_API LuaBinding_Math : public LuaBinding_Base
	{
		public:
			LuaBinding_Math(LuaBinding& binding);
			~LuaBinding_Math() = default;

			void Register(Nz::LuaInstance& instance) override;

			Nz::LuaClass<Nz::EulerAnglesd> eulerAngles;
			Nz::LuaClass<Nz::Matrix4d> matrix4d;
			Nz::LuaClass<Nz::Quaterniond> quaternion;
			Nz::LuaClass<Nz::Rectd> rect;
			Nz::LuaClass<Nz::Vector2d> vector2d;
			Nz::LuaClass<Nz::Vector3d> vector3d;
	};
}

#endif // NDK_LUABINDING_MATH_HPP

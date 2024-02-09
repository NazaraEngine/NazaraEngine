// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_PHYSICS3DBODY_HPP
#define NAZARA_PHYSICS3D_PHYSICS3DBODY_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Physics3D/Config.hpp>

namespace Nz
{
	class NAZARA_PHYSICS3D_API Physics3DBody
	{
		public:
			Physics3DBody() = default;
			Physics3DBody(const Physics3DBody&) = delete;
			Physics3DBody(Physics3DBody&&) = delete;
			virtual ~Physics3DBody();

			virtual UInt32 GetBodyIndex() const = 0;

			Physics3DBody& operator=(const Physics3DBody&) = delete;
			Physics3DBody& operator=(Physics3DBody&&) = delete;
	};
}

#include <Nazara/Physics3D/Physics3DBody.inl>

#endif // NAZARA_PHYSICS3D_PHYSICS3DBODY_HPP

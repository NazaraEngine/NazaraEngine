// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOLTPHYSICS3D_JOLTABSTRACTBODY_HPP
#define NAZARA_JOLTPHYSICS3D_JOLTABSTRACTBODY_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/JoltPhysics3D/Config.hpp>

namespace Nz
{
	class NAZARA_JOLTPHYSICS3D_API JoltAbstractBody
	{
		public:
			JoltAbstractBody() = default;
			JoltAbstractBody(const JoltAbstractBody&) = delete;
			JoltAbstractBody(JoltAbstractBody&&) = delete;
			virtual ~JoltAbstractBody();

			virtual UInt32 GetBodyIndex() const = 0;

			JoltAbstractBody& operator=(const JoltAbstractBody&) = delete;
			JoltAbstractBody& operator=(JoltAbstractBody&&) = delete;
	};
}

#include <Nazara/JoltPhysics3D/JoltAbstractBody.inl>

#endif // NAZARA_JOLTPHYSICS3D_JOLTABSTRACTBODY_HPP

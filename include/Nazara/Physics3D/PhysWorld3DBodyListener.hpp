// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_PhysWorld3DBodyListener_HPP
#define NAZARA_PHYSICS3D_PhysWorld3DBodyListener_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Physics3D/Export.hpp>

namespace Nz
{
	class NAZARA_PHYSICS3D_API PhysWorld3DBodyListener
	{
		public:
			PhysWorld3DBodyListener() = default;
			PhysWorld3DBodyListener(const PhysWorld3DBodyListener&) = delete;
			PhysWorld3DBodyListener(PhysWorld3DBodyListener&&) = delete;
			virtual ~PhysWorld3DBodyListener();

			virtual void OnBodyActivated(UInt32 bodyIndex);
			virtual void OnBodyDeactivated(UInt32 bodyIndex);

			PhysWorld3DBodyListener& operator=(const PhysWorld3DBodyListener&) = delete;
			PhysWorld3DBodyListener& operator=(PhysWorld3DBodyListener&&) = delete;
	};
}

#include <Nazara/Physics3D/PhysWorld3DBodyListener.inl>

#endif // NAZARA_PHYSICS3D_PhysWorld3DBodyListener_HPP

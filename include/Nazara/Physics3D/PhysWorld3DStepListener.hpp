// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_PHYSWORLD3DSTEPLISTENER_HPP
#define NAZARA_PHYSICS3D_PHYSWORLD3DSTEPLISTENER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Physics3D/Export.hpp>

namespace Nz
{
	class NAZARA_PHYSICS3D_API PhysWorld3DStepListener
	{
		public:
			PhysWorld3DStepListener() = default;
			PhysWorld3DStepListener(const PhysWorld3DStepListener&) = delete;
			PhysWorld3DStepListener(PhysWorld3DStepListener&&) = delete;
			virtual ~PhysWorld3DStepListener();

			virtual void PostSimulate(float elapsedTime);
			virtual void PreSimulate(float elapsedTime);

			PhysWorld3DStepListener& operator=(const PhysWorld3DStepListener&) = delete;
			PhysWorld3DStepListener& operator=(PhysWorld3DStepListener&&) = delete;
	};
}

#include <Nazara/Physics3D/PhysWorld3DStepListener.inl>

#endif // NAZARA_PHYSICS3D_PHYSWORLD3DSTEPLISTENER_HPP

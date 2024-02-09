// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_PHYSISCS3DSTEPLISTENER_HPP
#define NAZARA_PHYSICS3D_PHYSISCS3DSTEPLISTENER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Physics3D/Config.hpp>

namespace Nz
{
	class NAZARA_PHYSICS3D_API Physiscs3DStepListener
	{
		public:
			Physiscs3DStepListener() = default;
			Physiscs3DStepListener(const Physiscs3DStepListener&) = delete;
			Physiscs3DStepListener(Physiscs3DStepListener&&) = delete;
			virtual ~Physiscs3DStepListener();

			virtual void PostSimulate(float elapsedTime);
			virtual void PreSimulate(float elapsedTime);

			Physiscs3DStepListener& operator=(const Physiscs3DStepListener&) = delete;
			Physiscs3DStepListener& operator=(Physiscs3DStepListener&&) = delete;
	};
}

#include <Nazara/Physics3D/Physiscs3DStepListener.inl>

#endif // NAZARA_PHYSICS3D_PHYSISCS3DSTEPLISTENER_HPP

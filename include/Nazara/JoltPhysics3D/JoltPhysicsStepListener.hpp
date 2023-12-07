// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOLTPHYSICS3D_JOLTPHYSICSSTEPLISTENER_HPP
#define NAZARA_JOLTPHYSICS3D_JOLTPHYSICSSTEPLISTENER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/JoltPhysics3D/Config.hpp>

namespace Nz
{
	class NAZARA_JOLTPHYSICS3D_API JoltPhysicsStepListener
	{
		public:
			JoltPhysicsStepListener() = default;
			JoltPhysicsStepListener(const JoltPhysicsStepListener&) = delete;
			JoltPhysicsStepListener(JoltPhysicsStepListener&&) = delete;
			virtual ~JoltPhysicsStepListener();

			virtual void PostSimulate(float elapsedTime);
			virtual void PreSimulate(float elapsedTime);

			JoltPhysicsStepListener& operator=(const JoltPhysicsStepListener&) = delete;
			JoltPhysicsStepListener& operator=(JoltPhysicsStepListener&&) = delete;
	};
}

#include <Nazara/JoltPhysics3D/JoltPhysicsStepListener.inl>

#endif // NAZARA_JOLTPHYSICS3D_JOLTPHYSICSSTEPLISTENER_HPP

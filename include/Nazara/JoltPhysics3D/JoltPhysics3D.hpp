// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOLTPHYSICS3D_HPP
#define NAZARA_JOLTPHYSICS3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/JoltPhysics3D/Config.hpp>
#include <memory>

namespace JPH
{
	class JobSystem;
	class JobSystemThreadPool;
}

namespace Nz
{
	class NAZARA_JOLTPHYSICS3D_API JoltPhysics3D : public ModuleBase<JoltPhysics3D>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			struct Config {};

			JoltPhysics3D(Config /*config*/);
			~JoltPhysics3D();

			JPH::JobSystem& GetThreadPool();

		private:
			std::unique_ptr<JPH::JobSystemThreadPool> m_threadPool;

			static JoltPhysics3D* s_instance;
	};
}

#endif // NAZARA_JOLTPHYSICS3D_HPP

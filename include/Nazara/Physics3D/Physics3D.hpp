// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_HPP
#define NAZARA_PHYSICS3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <memory>

namespace JPH
{
	class JobSystem;
	class JobSystemThreadPool;
}

namespace Nz
{
	class NAZARA_PHYSICS3D_API Physics3D : public ModuleBase<Physics3D>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			struct Config {};

			Physics3D(Config /*config*/);
			~Physics3D();

			JPH::JobSystem& GetThreadPool();

		private:
			std::unique_ptr<JPH::JobSystemThreadPool> m_threadPool;

			static Physics3D* s_instance;
	};
}

#endif // NAZARA_PHYSICS3D_HPP

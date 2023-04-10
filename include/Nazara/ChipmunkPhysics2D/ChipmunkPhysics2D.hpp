// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CHIPMUNKPHYSICS2D_HPP
#define NAZARA_CHIPMUNKPHYSICS2D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/ChipmunkPhysics2D/Config.hpp>

namespace Nz
{
	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkPhysics2D : public ModuleBase<ChipmunkPhysics2D>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			struct Config {};

			ChipmunkPhysics2D(Config /*config*/);
			~ChipmunkPhysics2D() = default;

		private:
			static ChipmunkPhysics2D* s_instance;
	};
}

#endif // NAZARA_CHIPMUNKPHYSICS2D_HPP

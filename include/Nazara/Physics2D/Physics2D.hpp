// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS2D_HPP
#define NAZARA_PHYSICS2D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Physics2D/Export.hpp>

namespace Nz
{
	class NAZARA_PHYSICS2D_API Physics2D : public ModuleBase<Physics2D>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			struct Config {};

			Physics2D(Config /*config*/);
			~Physics2D() = default;

		private:
			static Physics2D* s_instance;
	};
}

#endif // NAZARA_PHYSICS2D_HPP

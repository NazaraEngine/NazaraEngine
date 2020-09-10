// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS2D_HPP
#define NAZARA_PHYSICS2D_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Physics2D/Config.hpp>

namespace Nz
{
	class NAZARA_PHYSICS2D_API Physics2D : public Module<Physics2D>
	{
		friend Module;

		public:
			using Dependencies = TypeList<Core>;

			Physics2D();
			~Physics2D() = default;

		private:
			static Physics2D* s_instance;
	};
}

#endif // NAZARA_PHYSICS2D_HPP

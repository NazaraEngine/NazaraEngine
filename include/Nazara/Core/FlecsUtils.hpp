// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_FLECSUTILS_HPP
#define NAZARA_CORE_FLECSUTILS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <flecs.h>

namespace Nz
{
	struct FlecsHasher
	{
		inline std::size_t operator()(flecs::entity entity) const;
	};

	class Flecs
	{
		public:
			static inline void EnsureInit();

		private:
			inline Flecs();
			inline ~Flecs();
	};
}

#include <Nazara/Core/FlecsUtils.inl>

#endif // NAZARA_CORE_FLECSUTILS_HPP

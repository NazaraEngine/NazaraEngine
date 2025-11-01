// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/Hash.hpp>

namespace Nz
{
	inline std::size_t FlecsHasher::operator()(flecs::entity entity) const
	{
		return HashCombine(entity.world().c_ptr(), entity.id());
	}

	Flecs::Flecs()
	{
		ecs_os_init();
	}

	Flecs::~Flecs()
	{
		ecs_os_fini();
	}

	void Flecs::EnsureInit()
	{
		static Flecs flecs;
	}
}

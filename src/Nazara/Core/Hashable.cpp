// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hashable.hpp>
#include <Nazara/Core/Hash.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	Hashable::~Hashable() = default;

	HashDigest Hashable::GetHash(HashType hash) const
	{
		Hash h(hash);
		return h.Process(*this);
	}

	HashDigest Hashable::GetHash(AbstractHash* impl) const
	{
		Hash h(impl);
		return h.Process(*this);
	}
}

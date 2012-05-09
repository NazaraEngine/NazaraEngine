// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Hashable.hpp>
#include <Nazara/Core/Hash.hpp>
#include <Nazara/Core/Debug.hpp>

NzHashable::~NzHashable() = default;

NzHashDigest NzHashable::GetHash(nzHash hash) const
{
	NzHash h(hash);
	return h.Hash(*this);
}

NzHashDigest NzHashable::GetHash(NzHashImpl* impl) const
{
	NzHash h(impl);
	return h.Hash(*this);
}

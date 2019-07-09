// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/EntityList.hpp>
#include <NDK/World.hpp>

namespace Ndk
{
	const EntityHandle& EntityList::iterator::operator*() const
	{
		return m_list->GetWorld()->GetEntity(static_cast<EntityId>(m_nextEntityId));
	}
}

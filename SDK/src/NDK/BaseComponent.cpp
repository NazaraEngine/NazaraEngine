// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/BaseComponent.hpp>

namespace Ndk
{
	BaseComponent::~BaseComponent() = default;

	nzUInt32 BaseComponent::s_nextId = 0;
}

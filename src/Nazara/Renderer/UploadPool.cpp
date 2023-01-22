// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/UploadPool.hpp>
#include <cassert>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	UploadPool::Allocation::~Allocation() = default;
}

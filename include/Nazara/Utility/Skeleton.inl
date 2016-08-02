// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	template<typename... Args>
	SkeletonRef Skeleton::New(Args&&... args)
	{
		std::unique_ptr<Skeleton> object(new Skeleton(std::forward<Args>(args)...));
		return object.release();
	}
}

#include <Nazara/Utility/DebugOff.hpp>

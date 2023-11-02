// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline FramePipelinePass::FramePipelinePass(FramePipelineNotificationFlags notificationFlags) :
	m_notificationFlags(notificationFlags)
	{
	}

	inline bool FramePipelinePass::ShouldNotify(FramePipelineNotification notification) const
	{
		return m_notificationFlags.Test(notification);
	}
}

#include <Nazara/Graphics/DebugOff.hpp>

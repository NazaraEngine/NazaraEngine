// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp


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


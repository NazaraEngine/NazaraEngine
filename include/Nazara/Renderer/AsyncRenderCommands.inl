// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline void AsyncRenderCommands::AddCompletionCallback(CompletionCallback&& callback)
	{
		m_completionCallbacks.push_back(std::move(callback));
	}

	inline void AsyncRenderCommands::TriggerCallbacks()
	{
		for (CompletionCallback& callback : m_completionCallbacks)
			callback();
	}
}


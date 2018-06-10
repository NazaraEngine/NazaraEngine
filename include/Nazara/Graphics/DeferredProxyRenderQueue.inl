// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DeferredProxyRenderQueue.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a DeferredProxyRenderQueue using a deferred and a forward queues
	*
	* \param deferredQueue Deferred queue which will be used for non-blended objects
	* \param forwardQueue Forward queue which will be used for blended objects
	*/
	inline DeferredProxyRenderQueue::DeferredProxyRenderQueue(BasicRenderQueue* deferredQueue, BasicRenderQueue* forwardQueue) :
	m_deferredRenderQueue(deferredQueue),
	m_forwardRenderQueue(forwardQueue)
	{
	}

	inline BasicRenderQueue* DeferredProxyRenderQueue::GetDeferredRenderQueue()
	{
		return m_deferredRenderQueue;
	}

	inline BasicRenderQueue* DeferredProxyRenderQueue::GetForwardRenderQueue()
	{
		return m_forwardRenderQueue;
	}
}

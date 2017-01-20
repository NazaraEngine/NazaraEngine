// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleFunctionRenderer.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline ParticleFunctionRenderer::ParticleFunctionRenderer(Renderer renderer) :
	m_renderer(std::move(renderer))
	{
	}

	/*!
	* \brief Gets the renderer function
	*
	* \return Renderer function responsible for particle rendering
	*/
	inline const ParticleFunctionRenderer::Renderer& ParticleFunctionRenderer::GetRenderer() const
	{
		return m_renderer;
	}

	/*!
	* \brief Sets the renderer function
	*
	* \remark The renderer function must be valid
	*/
	inline void ParticleFunctionRenderer::SetRenderer(Renderer renderer)
	{
		m_renderer = std::move(renderer);
	}

	template<typename... Args>
	ParticleFunctionRendererRef ParticleFunctionRenderer::New(Args&&... args)
	{
		std::unique_ptr<ParticleFunctionRenderer> object(new ParticleFunctionRenderer(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>

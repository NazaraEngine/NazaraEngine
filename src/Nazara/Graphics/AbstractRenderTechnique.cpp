// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/RenderTechniques.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	AbstractRenderTechnique::AbstractRenderTechnique()
	{
		#ifdef NAZARA_DEBUG
		if (!Renderer::IsInitialized())
		{
			NazaraError("NazaraRenderer is not initialized");
			return;
		}
		#endif

		m_instancingEnabled = Renderer::HasCapability(RendererCap_Instancing);
	}

	AbstractRenderTechnique::~AbstractRenderTechnique() = default;

	void AbstractRenderTechnique::EnableInstancing(bool instancing)
	{
		if (Renderer::HasCapability(RendererCap_Instancing))
			m_instancingEnabled = instancing;
		else if (instancing)
			NazaraError("NazaraRenderer does not support instancing");
	}

	String AbstractRenderTechnique::GetName() const
	{
		return RenderTechniques::ToString(GetType());
	}

	bool AbstractRenderTechnique::IsInstancingEnabled() const
	{
		return m_instancingEnabled;
	}
}

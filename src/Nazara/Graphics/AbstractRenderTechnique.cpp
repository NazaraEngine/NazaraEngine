// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/RenderTechniques.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzAbstractRenderTechnique::NzAbstractRenderTechnique()
{
	#ifdef NAZARA_DEBUG
	if (!NzRenderer::IsInitialized())
	{
		NazaraError("NazaraRenderer is not initialized");
		return;
	}
	#endif

	m_instancingEnabled = NzRenderer::HasCapability(nzRendererCap_Instancing);
}

NzAbstractRenderTechnique::~NzAbstractRenderTechnique() = default;

void NzAbstractRenderTechnique::EnableInstancing(bool instancing)
{
	if (NzRenderer::HasCapability(nzRendererCap_Instancing))
		m_instancingEnabled = instancing;
	else if (instancing)
		NazaraError("NazaraRenderer does not support instancing");
}

NzString NzAbstractRenderTechnique::GetName() const
{
	return NzRenderTechniques::ToString(GetType());
}

bool NzAbstractRenderTechnique::IsInstancingEnabled() const
{
	return m_instancingEnabled;
}

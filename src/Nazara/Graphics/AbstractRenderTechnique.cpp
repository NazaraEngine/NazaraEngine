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
	AbstractRenderTechnique::AbstractRenderTechnique() :
	m_instancingEnabled(true)
	{
	}

	AbstractRenderTechnique::~AbstractRenderTechnique() = default;

	void AbstractRenderTechnique::EnableInstancing(bool instancing)
	{
		m_instancingEnabled = instancing;
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

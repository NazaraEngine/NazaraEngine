// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/RenderTechniques.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzAbstractRenderTechnique::NzAbstractRenderTechnique() :
m_instancingEnabled(true)
{
}

NzAbstractRenderTechnique::~NzAbstractRenderTechnique() = default;

void NzAbstractRenderTechnique::EnableInstancing(bool instancing)
{
	m_instancingEnabled = instancing;
}

NzString NzAbstractRenderTechnique::GetName() const
{
	return NzRenderTechniques::ToString(GetType());
}

bool NzAbstractRenderTechnique::IsInstancingEnabled() const
{
	return m_instancingEnabled;
}

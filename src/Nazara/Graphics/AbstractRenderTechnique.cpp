// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/RenderTechniques.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::AbstractRenderTechnique
	* \brief Graphics class that represents the rendering technique for our scene
	*
	* \remark This class is abstract
	*/

	/*!
	* \brief Constructs a AbstractRenderTechnique object
	*/

	AbstractRenderTechnique::AbstractRenderTechnique() :
	m_instancingEnabled(true)
	{
	}

	AbstractRenderTechnique::~AbstractRenderTechnique() = default;

	/*!
	* \brief Enables the instancing
	*
	* \param instancing Should instancing be enabled
	*
	* \remark This may improve performances
	*/

	void AbstractRenderTechnique::EnableInstancing(bool instancing)
	{
		m_instancingEnabled = instancing;
	}

	/*!
	* \brief Gets the name of the actual technique
	* \return Name of the technique being used
	*/

	String AbstractRenderTechnique::GetName() const
	{
		return RenderTechniques::ToString(GetType());
	}

	/*!
	* \brief Checks whether the instancing is enabled
	* \return true If it is the case
	*/

	bool AbstractRenderTechnique::IsInstancingEnabled() const
	{
		return m_instancingEnabled;
	}
}

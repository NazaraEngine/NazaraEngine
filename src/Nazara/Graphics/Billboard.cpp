// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Billboard.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::Billboard
	* \brief Graphics class that represents a billboard, a 2D surface which simulates a 3D object
	*/

	/*!
	* \brief Adds this billboard to the render queue
	*
	* \param renderQueue Queue to be added
	* \param instanceData Data used for instance
	*/

	void Billboard::AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData) const
	{
		Nz::Vector3f position = instanceData.transformMatrix.GetTranslation();
		renderQueue->AddBillboards(instanceData.renderOrder, GetMaterial(), 1, &position, &m_size, &m_sinCos, &m_color);
	}

	/*
	* \brief Makes the bounding volume of this billboard
	*/

	void Billboard::MakeBoundingVolume() const
	{
		// As billboard always face the screen, we must take its maximum size in account on every axis
		float maxSize = float(M_SQRT2) * std::max(m_size.x, m_size.y);

		Nz::Vector3f halfSize = (maxSize * Vector3f::Right() + maxSize * Vector3f::Down() + maxSize * Vector3f::Forward()) / 2.f;

		m_boundingVolume.Set(-halfSize, halfSize);
	}

	BillboardLibrary::LibraryMap Billboard::s_library;
}

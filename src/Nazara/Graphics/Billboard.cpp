// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Billboard.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <cstring>
#include <memory>
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
		if (!m_material)
			return;

		Nz::Vector3f position = instanceData.transformMatrix->GetTranslation();
		renderQueue->AddBillboards(instanceData.renderOrder, m_material, 1, &position, &m_size, &m_sinCos, &m_color);
	}

	/*
	* \brief Makes the bounding volume of this billboard
	*/

	void Billboard::MakeBoundingVolume() const
	{
		constexpr float sqrt2 = float(M_SQRT2);

		m_boundingVolume.Set(Vector3f(0.f), sqrt2 * m_size.x * Vector3f::Right() + sqrt2 * m_size.y * Vector3f::Down());
	}

	BillboardLibrary::LibraryMap Billboard::s_library;
}

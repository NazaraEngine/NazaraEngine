// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a Model object by default
	*/
	Model::Model()
	{
		ResetMaterials(0);
	}

	/*!
	* \brief Adds this model to a render queue, using user-specified transform matrix and render order
	*
	* This can be useful when drawing particles
	*
	* \param renderQueue Queue to be added
	* \param transformMatrix Transform matrix to be used for rendering the model
	* \param renderOrder Specify the render queue layer to be used
	* \param scissorRect The Scissor rect to uses for rendering
	*/
	void Model::AddToRenderQueue(AbstractRenderQueue* renderQueue, const Matrix4f& transformMatrix, int renderOrder, const Recti& scissorRect) const
{
		InstanceData instanceData(Nz::Matrix4f::Identity());
		instanceData.renderOrder = renderOrder;
		instanceData.transformMatrix = transformMatrix;
		return AddToRenderQueue(renderQueue, instanceData, scissorRect);
	}

	/*!
	* \brief Creates a new Model from the arguments
	* \return A reference to the newly created model
	*
	* \param args Arguments for the model
	*/

	template<typename... Args>
	ModelRef Model::New(Args&&... args)
	{
		std::unique_ptr<Model> object(new Model(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>

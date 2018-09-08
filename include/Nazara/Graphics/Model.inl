// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Model.hpp>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a Model object by default
	*/
	inline Model::Model()
	{
		ResetMaterials(0);
	}

	/*!
	* \brief Constructs a Model object by copying another
	*
	* \param model Model to copy
	*/
	inline Model::Model(const Model& model) :
	InstancedRenderable(model)
	{
		SetMesh(model.m_mesh);
		
		// Since SetMesh does reset materials, we need reapply them
		SetSkinCount(model.GetSkinCount());
		for (std::size_t skin = 0; skin < model.GetSkinCount(); ++skin)
		{
			for (std::size_t matIndex = 0; matIndex < model.GetMaterialCount(); ++matIndex)
				SetMaterial(skin, matIndex, model.GetMaterial(skin, matIndex));
		}
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
	inline void Model::AddToRenderQueue(AbstractRenderQueue* renderQueue, const Matrix4f& transformMatrix, int renderOrder, const Recti& scissorRect) const
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

// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline ParticleFunctionController::ParticleFunctionController(Controller controller) :
	m_controller(std::move(controller))
	{
	}

	/*!
	* \brief Gets the controller function
	*
	* \return Controller function responsible for particle update
	*/
	inline const ParticleFunctionController::Controller& ParticleFunctionController::GetController() const
	{
		return m_controller;
	}

	/*!
	* \brief Sets the controller function
	*
	* \remark The controller function must be valid
	*/
	inline void ParticleFunctionController::SetController(Controller controller)
	{
		m_controller = std::move(controller);
	}

	template<typename... Args>
	ParticleFunctionControllerRef ParticleFunctionController::New(Args&&... args)
	{
		std::unique_ptr<ParticleFunctionController> object(new ParticleFunctionController(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>

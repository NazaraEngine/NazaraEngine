// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

namespace Ndk
{
	/*!
	* \brief Constructs an LightComponent object with a light type
	*/

	inline LightComponent::LightComponent(Nz::LightType lightType) :
	Nz::Light(lightType)
	{
	}
}

#pragma once

#include <Nazara/Utility/Algorithm.hpp>

namespace Nz
{
	template<typename T>
	ComponentType ComponentTypeOf()
	{
		NazaraAssert(false, "This type is not supported !");
		return ComponentType(-1);
	}
}

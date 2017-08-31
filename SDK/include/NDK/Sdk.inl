// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	/*!
	* \brief Checks whether the module is initialized
	* \return true if module is initialized
	*/

	inline bool Sdk::IsInitialized()
	{
		return s_referenceCounter != 0;
	}
}

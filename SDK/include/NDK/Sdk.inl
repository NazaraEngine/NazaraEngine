// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	inline bool Sdk::IsInitialized()
	{
		return s_referenceCounter != 0;
	}
}

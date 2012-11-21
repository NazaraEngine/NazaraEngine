// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Loaders/MD5Anim.hpp>
#include <Nazara/Utility/Loaders/MD5Anim/Parser.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	bool Check(NzInputStream& stream, const NzAnimationParams& parameters)
	{
		NzMD5AnimParser parser(stream, parameters);
		return parser.Check();
	}

	bool Load(NzAnimation* animation, NzInputStream& stream, const NzAnimationParams& parameters)
	{
		NzMD5AnimParser parser(stream, parameters);
		return parser.Parse(animation);
	}
}

void NzLoaders_MD5Anim_Register()
{
	NzAnimationLoader::RegisterLoader("md5anim", Check, Load);
}

void NzLoaders_MD5Anim_Unregister()
{
	NzAnimationLoader::UnregisterLoader("md5anim", Check, Load);
}

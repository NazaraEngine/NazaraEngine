// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENUMS_2D_HPP
#define NAZARA_ENUMS_2D_HPP

enum nzBackgroundType
{
	nzBackgroundType_Color,   // NzColorBackground
	nzBackgroundType_Skybox,  // NzSkyboxBackground
	nzBackgroundType_Texture, // NzTextureBackground
	nzBackgroundType_User,

	nzBackgroundType_Max = nzBackgroundType_User
};

#endif // NAZARA_ENUMS_2D_HPP

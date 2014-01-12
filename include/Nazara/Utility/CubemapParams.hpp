// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CUBEMAPPARAMS_HPP
#define NAZARA_CUBEMAPPARAMS_HPP

#include <Nazara/Math/Vector2.hpp>

struct NzCubemapParams
{
	/*
	La position de chaque face dans la cubemap
	Les indices ici seront multipliés à la taille d'une face pour obtenir le coin haut-gauche de la zone.
	Si la taille d'une face est 0, elle sera calculée via max(width, height)/4.

	Par défaut, cela suit ce layout :
	  U
	L F R B
	  D

	Si ce n'est pas le cas, à vous de repositionner les faces correctement.
	*/
	NzVector2ui backPosition = NzVector2ui(3, 1);
	NzVector2ui downPosition = NzVector2ui(1, 2);
	NzVector2ui forwardPosition = NzVector2ui(1, 1);
	NzVector2ui leftPosition = NzVector2ui(0, 1);
	NzVector2ui rightPosition = NzVector2ui(2, 1);
	NzVector2ui upPosition = NzVector2ui(1, 0);
	unsigned int faceSize = 0;
};

#endif // NAZARA_CUBEMAPPARAMS_HPP

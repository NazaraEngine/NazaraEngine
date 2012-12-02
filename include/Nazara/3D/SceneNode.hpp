// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SCENENODE_HPP
#define NAZARA_SCENENODE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/3D/Enums.hpp>
#include <Nazara/Utility/AxisAlignedBox.hpp>
#include <Nazara/Utility/Node.hpp>

class NzSceneNode : public NzNode
{
	public:
		virtual ~NzSceneNode();

		virtual const NzAxisAlignedBox& GetAABB() const = 0;
		virtual nzSceneNodeType GetSceneNodeType() const = 0;
};

#endif // NAZARA_SCENENODE_HPP

// Copyright (C) 2014 Gawaboumga (Youri Hubaut) - Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RAYPICKING_HPP
#define NAZARA_RAYPICKING_HPP

#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/SceneNode.hpp>
#include <Nazara/Math/Ray.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Node.hpp>
#include <map>
#include <limits>



		// I think the name of the methods could be longer :)



//! NzRayPicking provides methods to find NzSceneNodes below the mouse position.
/** Methods offers to find each NzSceneNodes or only one in your scene based on the mouse position on the screen.
It can also do the opposite: project a 3D position on the screen to get the mouse coordinates. */
class NAZARA_API NzRayPicking
{
	public:
		NzRayPicking() = default;
		~NzRayPicking() = default;

		//! Returns a map of each nodes who is below the mouse position (key: distance -> nearest to the farthest).
		/** Nodes, that are traversed by the ray emitted by camera in the direction of the mouse position, are stocked in a
		map whose key is the distance between the first collision (with AABB test) and the camera. Map are sorted by std::less,
		thus closer the object is, closer to the beginning of the map will be.
			\param mousePosition: mouse position (not normalized).
			\param camera: active camera.
			\param root: which children will be test.
			\param maxDistance: maximal distance for the collision point.
			\return map of sorted NzSceneNode by distance. */
		static std::map<float, NzSceneNode*> GetEverySceneNodeFromMousePosition(const NzVector2i& mousePosition, const NzCamera& camera, const NzNode* root, float maxDistance = std::numeric_limits<float>::infinity());

		//! Projects a 3D position on a mouse position.
		/** There are rounding issues (edges of the screen can have overflowed values).
			\param position3D: position to project.
			\param camera: active camera.
			\return mouse position (not normalized). */
		static NzVector2i GetMousePositionFrom3DCoordinates(const NzVector3f& position3D, const NzCamera& camera);

		//! Returns the ray emitted by camera in the direction of the mouse position.
		/** Mainly used for GetEverySceneNodeFromMousePosition() and GetSceneNodeFromMousePosition().
			\param mousePosition: mouse position (not normalized).
			\param camera: active camera.
			\return ray emitted. */
		static NzRayf GetRayFromMousePosition(const NzVector2i& mousePosition, const NzCamera& camera);

		//! Returns the first hit node who is below the mouse position.
		/** First hit node (with AABB test)
			\param mousePosition: mouse position (not normalized).
			\param camera: active camera.
			\param root: which children will be test.
			\param maxDistance: maximal distance for the collision point.
			\return first NzSceneNode hit. */
		static NzSceneNode* GetSceneNodeFromMousePosition(const NzVector2i& mousePosition, const NzCamera& camera, const NzNode* root, float maxDistance = std::numeric_limits<float>::infinity());

	private:
		static void RecursiveRayPicking(const NzRayf& ray, const NzCamera& camera, const NzNode* root);

		static std::map<float, NzSceneNode*> m_lastPicked;
		static float m_distance;
		static float m_maxDistance;
};

#endif // NAZARA_RAYPICKING_HPP

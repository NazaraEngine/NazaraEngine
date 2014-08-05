// Copyright (C) 2014 Gawaboumga (Youri Hubaut) - Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/RayPicking.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Graphics/Debug.hpp>

std::map<float, NzSceneNode*> NzRayPicking::m_lastPicked = std::map<float, NzSceneNode*>();
float NzRayPicking::m_distance = std::numeric_limits<float>::infinity();
float NzRayPicking::m_maxDistance = std::numeric_limits<float>::infinity();

std::map<float, NzSceneNode*> NzRayPicking::GetEverySceneNodeFromMousePosition(const NzVector2i& mousePosition, const NzCamera& camera, const NzNode* root, float maxDistance)
{
	NzRayf ray = GetRayFromMousePosition(mousePosition, camera);

	m_maxDistance = maxDistance;
	m_lastPicked.clear();

	RecursiveRayPicking(ray, camera, root);

	return m_lastPicked;
}

NzVector2i NzRayPicking::GetMousePositionFrom3DCoordinates(const NzVector3f& position3D, const NzCamera& camera)
{
	NzVector3f transformedPosition = (camera.GetViewMatrix() * camera.GetProjectionMatrix()).Transform(position3D);

	if (transformedPosition.z < 0.f)
	{
		#ifdef NAZARA_DEBUG
		NazaraError("Failed to transform 3D coordinates to screen coordinates");
		#endif

		return NzVector2i(std::numeric_limits<int>::infinity(), std::numeric_limits<int>::infinity()); // What to do ?
	}
	else
	{
		NzVector2f viewPort = NzVector2f(camera.GetViewport().GetLengths()) / 2.f;

		float zDepth = 1.f;
		if (!NzNumberEquals(transformedPosition.z, 0.f))
			zDepth = 1.f / transformedPosition.z; // precision error

		return NzVector2i(viewPort.x + viewPort.x * transformedPosition.x * zDepth,
						  viewPort.y - viewPort.y * transformedPosition.y * zDepth);
	}
}

NzRayf NzRayPicking::GetRayFromMousePosition(const NzVector2i& mousePosition, const NzCamera& camera)
{
	NzVector3f view = camera.GetForward();
	view.Normalize();

	NzVector3f horizontal = camera.GetRight();
	horizontal.Normalize();

	NzVector3f vertical = NzVector3f::CrossProduct(horizontal, view); // camera.GetUp() ?
	vertical.Normalize();

	vertical *= (camera.GetFrustum().GetCorner(nzCorner_NearLeftBottom) - camera.GetFrustum().GetCorner(nzCorner_NearLeftTop)).GetLength(); // Real vertical length of the frustum's near plane.
	horizontal *= (camera.GetFrustum().GetCorner(nzCorner_NearLeftBottom) - camera.GetFrustum().GetCorner(nzCorner_NearRightBottom)).GetLength(); // Real horizontal length of the frustum's near plane.

	float x = mousePosition.x;
	float y = mousePosition.y;

	x /= camera.GetViewport().width;
	y /= camera.GetViewport().height; // mouse position "normalized"

	NzRayf ray;
	ray.origin = camera.GetPosition();
	// We convert upper left coordinates to the middle ones.
	ray.direction = ((x - 0.5f) * horizontal + ((1.f - 2.f * y)/2.f) * vertical + view * camera.GetZNear()).Normalize();

	return ray;
}

NzSceneNode* NzRayPicking::GetSceneNodeFromMousePosition(const NzVector2i& mousePosition, const NzCamera& camera, const NzNode* root, float maxDistance)
{
	NzRayf ray = GetRayFromMousePosition(mousePosition, camera);

	m_maxDistance = maxDistance;
	m_lastPicked.clear();

	RecursiveRayPicking(ray, camera, root);

	if (m_lastPicked.empty())
		return nullptr;
	else
		return m_lastPicked.begin()->second;
}

void NzRayPicking::RecursiveRayPicking(const NzRayf& ray, const NzCamera& camera, const NzNode* node)
{
	for (NzNode* child : node->GetChilds())
	{
		if (child->GetNodeType() == nzNodeType_Scene)
		{
			NzSceneNode* sceneNode = static_cast<NzSceneNode*>(child);

			if (sceneNode->IsVisible())
			{
				float closestPoint;
				float farthestPoint;

				if (ray.Intersect(sceneNode->GetBoundingVolume().aabb, &closestPoint, &farthestPoint))
				{
					if (!m_lastPicked.empty())
					{
						float minDistance = std::min(closestPoint, farthestPoint);

						if (minDistance < m_maxDistance)
						{
							m_distance = minDistance;
							m_lastPicked[m_distance] = sceneNode;
						}
					}
					else
					{
						float minDistance = std::min(std::min(m_distance, closestPoint), std::min(m_distance, farthestPoint));

						if (minDistance < m_maxDistance)
						{
							m_distance = minDistance;
							m_lastPicked[m_distance] = sceneNode;
						}
					}
				}
			}
		}

		if (child->HasChilds())
			RecursiveRayPicking(ray, camera, node);
	}
}

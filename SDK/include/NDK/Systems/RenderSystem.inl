// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	inline RenderSystem::RenderSystem(const RenderSystem& renderSystem) :
	System(renderSystem)
	{
	}

	inline const NzBackgroundRef& RenderSystem::GetDefaultBackground() const
	{
		return m_background;
	}

	inline const NzMatrix4f& RenderSystem::GetCoordinateSystemMatrix() const
	{
		return m_coordinateSystemMatrix;
	}

	inline NzVector3f RenderSystem::GetGlobalForward() const
	{
		return NzVector3f(-m_coordinateSystemMatrix.m13, -m_coordinateSystemMatrix.m23, -m_coordinateSystemMatrix.m33);
	}

	inline NzVector3f RenderSystem::GetGlobalRight() const
	{
		return NzVector3f(m_coordinateSystemMatrix.m11, m_coordinateSystemMatrix.m21, m_coordinateSystemMatrix.m31);
	}

	inline NzVector3f RenderSystem::GetGlobalUp() const
	{
		return NzVector3f(m_coordinateSystemMatrix.m12, m_coordinateSystemMatrix.m22, m_coordinateSystemMatrix.m32);
	}

	inline void RenderSystem::SetDefaultBackground(NzBackgroundRef background)
	{
		m_background = std::move(background);
	}

	inline void RenderSystem::SetGlobalForward(const NzVector3f& direction)
	{
		m_coordinateSystemMatrix.m13 = -direction.x;
		m_coordinateSystemMatrix.m23 = -direction.y;
		m_coordinateSystemMatrix.m33 = -direction.z;

		InvalidateCoordinateSystem();
	}

	inline void RenderSystem::SetGlobalRight(const NzVector3f& direction)
	{
		m_coordinateSystemMatrix.m11 = direction.x;
		m_coordinateSystemMatrix.m21 = direction.y;
		m_coordinateSystemMatrix.m31 = direction.z;

		InvalidateCoordinateSystem();
	}

	inline void RenderSystem::SetGlobalUp(const NzVector3f& direction)
	{
		m_coordinateSystemMatrix.m12 = direction.x;
		m_coordinateSystemMatrix.m22 = direction.y;
		m_coordinateSystemMatrix.m32 = direction.z;

		InvalidateCoordinateSystem();
	}

	inline void RenderSystem::InvalidateCoordinateSystem()
	{
		m_coordinateSystemInvalidated = true;
	}
}

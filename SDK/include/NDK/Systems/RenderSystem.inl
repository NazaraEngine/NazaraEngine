// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	inline RenderSystem::RenderSystem(const RenderSystem& renderSystem) :
	System(renderSystem)
	{
	}

	template<typename T>
	inline T& RenderSystem::ChangeRenderTechnique()
	{
		return *static_cast<T*>(ChangeRenderTechnique(std::make_unique<T>()));
	}

	inline Nz::AbstractRenderTechnique& RenderSystem::ChangeRenderTechnique(std::unique_ptr<Nz::AbstractRenderTechnique>&& renderTechnique)
	{
		m_renderTechnique = std::move(renderTechnique);
        return *m_renderTechnique.get();
	}

	inline const Nz::BackgroundRef& RenderSystem::GetDefaultBackground() const
	{
		return m_background;
	}

	inline const Nz::Matrix4f& RenderSystem::GetCoordinateSystemMatrix() const
	{
		return m_coordinateSystemMatrix;
	}

	inline Nz::Vector3f RenderSystem::GetGlobalForward() const
	{
		return Nz::Vector3f(-m_coordinateSystemMatrix.m13, -m_coordinateSystemMatrix.m23, -m_coordinateSystemMatrix.m33);
	}

	inline Nz::Vector3f RenderSystem::GetGlobalRight() const
	{
		return Nz::Vector3f(m_coordinateSystemMatrix.m11, m_coordinateSystemMatrix.m21, m_coordinateSystemMatrix.m31);
	}

	inline Nz::Vector3f RenderSystem::GetGlobalUp() const
	{
		return Nz::Vector3f(m_coordinateSystemMatrix.m12, m_coordinateSystemMatrix.m22, m_coordinateSystemMatrix.m32);
	}

	inline Nz::AbstractRenderTechnique& RenderSystem::GetRenderTechnique() const
	{
		return *m_renderTechnique.get();
	}

	inline void RenderSystem::SetDefaultBackground(Nz::BackgroundRef background)
	{
		m_background = std::move(background);
	}

	inline void RenderSystem::SetGlobalForward(const Nz::Vector3f& direction)
	{
		m_coordinateSystemMatrix.m13 = -direction.x;
		m_coordinateSystemMatrix.m23 = -direction.y;
		m_coordinateSystemMatrix.m33 = -direction.z;

		InvalidateCoordinateSystem();
	}

	inline void RenderSystem::SetGlobalRight(const Nz::Vector3f& direction)
	{
		m_coordinateSystemMatrix.m11 = direction.x;
		m_coordinateSystemMatrix.m21 = direction.y;
		m_coordinateSystemMatrix.m31 = direction.z;

		InvalidateCoordinateSystem();
	}

	inline void RenderSystem::SetGlobalUp(const Nz::Vector3f& direction)
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

// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	/*!
	* \brief Constructs a RenderSystem object by copy semantic
	*
	* \param renderSystem RenderSystem to copy
	*/

	inline RenderSystem::RenderSystem(const RenderSystem& renderSystem) :
	System(renderSystem)
	{
	}

	/*!
	* \brief Changes the render technique used for the system
	* \return A reference to the render technique type
	*/

	template<typename T>
	inline T& RenderSystem::ChangeRenderTechnique()
	{
		static_assert(std::is_base_of<Nz::AbstractRenderTechnique, T>::value, "RenderTechnique is not a subtype of AbstractRenderTechnique");
		return static_cast<T&>(ChangeRenderTechnique(std::make_unique<T>()));
	}

	/*!
	* \brief Changes the render technique used for the system
	* \return A reference to the abstract render technique
	*
	* \param renderTechnique Render technique to use
	*/

	inline Nz::AbstractRenderTechnique& RenderSystem::ChangeRenderTechnique(std::unique_ptr<Nz::AbstractRenderTechnique>&& renderTechnique)
	{
		m_renderTechnique = std::move(renderTechnique);
        return *m_renderTechnique.get();
	}

	/*!
	* \brief Gets the background used for rendering
	* \return A reference to the background
	*/

	inline const Nz::BackgroundRef& RenderSystem::GetDefaultBackground() const
	{
		return m_background;
	}

	/*!
	* \brief Gets the coordinates matrix used for rendering
	* \return A constant reference to the matrix of coordinates
	*/

	inline const Nz::Matrix4f& RenderSystem::GetCoordinateSystemMatrix() const
	{
		return m_coordinateSystemMatrix;
	}

	/*!
	* \brief Gets the "forward" global direction
	* \return The forward direction, by default, it's -UnitZ() (Right hand coordinates)
	*/

	inline Nz::Vector3f RenderSystem::GetGlobalForward() const
	{
		return Nz::Vector3f(-m_coordinateSystemMatrix.m13, -m_coordinateSystemMatrix.m23, -m_coordinateSystemMatrix.m33);
	}

	/*!
	* \brief Gets the "right" global direction
	* \return The right direction, by default, it's UnitX() (Right hand coordinates)
	*/

	inline Nz::Vector3f RenderSystem::GetGlobalRight() const
	{
		return Nz::Vector3f(m_coordinateSystemMatrix.m11, m_coordinateSystemMatrix.m21, m_coordinateSystemMatrix.m31);
	}

	/*!
	* \brief Gets the "up" global direction
	* \return The up direction, by default, it's UnitY() (Right hand coordinates)
	*/

	inline Nz::Vector3f RenderSystem::GetGlobalUp() const
	{
		return Nz::Vector3f(m_coordinateSystemMatrix.m12, m_coordinateSystemMatrix.m22, m_coordinateSystemMatrix.m32);
	}

	/*!
	* \brief Gets the render technique used for rendering
	* \return A reference to the abstract render technique being used
	*/

	inline Nz::AbstractRenderTechnique& RenderSystem::GetRenderTechnique() const
	{
		return *m_renderTechnique.get();
	}

	/*!
	* \brief Sets the background used for rendering
	*
	* \param background A reference to the background
	*/

	inline void RenderSystem::SetDefaultBackground(Nz::BackgroundRef background)
	{
		m_background = std::move(background);
	}

	/*!
	* \brief Sets the "forward" global direction
	*
	* \param direction The new forward direction
	*/

	inline void RenderSystem::SetGlobalForward(const Nz::Vector3f& direction)
	{
		m_coordinateSystemMatrix.m13 = -direction.x;
		m_coordinateSystemMatrix.m23 = -direction.y;
		m_coordinateSystemMatrix.m33 = -direction.z;

		InvalidateCoordinateSystem();
	}

	/*!
	* \brief Sets the "right" global direction
	*
	* \param direction The new right direction
	*/

	inline void RenderSystem::SetGlobalRight(const Nz::Vector3f& direction)
	{
		m_coordinateSystemMatrix.m11 = direction.x;
		m_coordinateSystemMatrix.m21 = direction.y;
		m_coordinateSystemMatrix.m31 = direction.z;

		InvalidateCoordinateSystem();
	}

	/*!
	* \brief Sets the "up" global direction
	*
	* \param direction The new up direction
	*/

	inline void RenderSystem::SetGlobalUp(const Nz::Vector3f& direction)
	{
		m_coordinateSystemMatrix.m12 = direction.x;
		m_coordinateSystemMatrix.m22 = direction.y;
		m_coordinateSystemMatrix.m32 = direction.z;

		InvalidateCoordinateSystem();
	}

	/*!
	* \brief Invalidates the matrix of coordinates for the system
	*/

	inline void RenderSystem::InvalidateCoordinateSystem()
	{
		m_coordinateSystemInvalidated = true;
	}
}

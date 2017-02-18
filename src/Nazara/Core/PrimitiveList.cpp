// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::PrimitiveList
	* \brief Core class that represents a list of geometric primitives
	*/

	/*!
	* \brief Adds a box centered
	*
	* \param lengths (Width, Height, Depht)
	* \param subdivision Number of subdivision for the axis
	* \param transformMatrix Matrix to apply
	*/

	void PrimitiveList::AddBox(const Vector3f& lengths, const Vector3ui& subdivision, const Matrix4f& transformMatrix)
	{
		m_primitives.push_back(Primitive::Box(lengths, subdivision, transformMatrix));
	}

	/*!
	* \brief Adds a box centered
	*
	* \param lengths (Width, Height, Depht)
	* \param subdivision Number of subdivision for the axis
	* \param position Position of the box
	* \param rotation Rotation of the box
	*/

	void PrimitiveList::AddBox(const Vector3f& lengths, const Vector3ui& subdivision, const Vector3f& position, const Quaternionf& rotation)
	{
		m_primitives.push_back(Primitive::Box(lengths, subdivision, position, rotation));
	}

	/*!
	* \brief Adds a cone, centered in (0, 0, 0) and circle in (0, -length, 0)
	*
	* \param length Height of the cone
	* \param radius Width of the radius
	* \param subdivision Number of sides for the circle
	* \param transformMatrix Matrix to apply
	*/

	void PrimitiveList::AddCone(float length, float radius, unsigned int subdivision, const Matrix4f& transformMatrix)
	{
		m_primitives.push_back(Primitive::Cone(length, radius, subdivision, transformMatrix));
	}

	/*!
	* \brief Adds a cone, centered in (0, 0, 0) and circle in (0, -length, 0)
	*
	* \param length Height of the cone
	* \param radius Width of the radius
	* \param subdivision Number of sides for the circle
	* \param position Position of the cone
	* \param rotation Rotation of the cone
	*/

	void PrimitiveList::AddCone(float length, float radius, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation)
	{
		m_primitives.push_back(Primitive::Cone(length, radius, subdivision, position, rotation));
	}

	/*!
	* \brief Adds a cubic sphere, centered in (0, 0, 0)
	*
	* \param size Radius of the cubic sphere
	* \param subdivision Number of subdivision for the box
	* \param transformMatrix Matrix to apply
	*/

	void PrimitiveList::AddCubicSphere(float size, unsigned int subdivision, const Matrix4f& transformMatrix)
	{
		m_primitives.push_back(Primitive::CubicSphere(size, subdivision, transformMatrix));
	}

	/*!
	* \brief Adds a cubic sphere, centered in (0, 0, 0)
	*
	* \param size Radius of the cubic sphere
	* \param subdivision Number of subdivision for the box
	* \param position Position of the cubic sphere
	* \param rotation Rotation of the cubic sphere
	*/

	void PrimitiveList::AddCubicSphere(float size, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation)
	{
		m_primitives.push_back(Primitive::CubicSphere(size, subdivision, position, rotation));
	}

	/*!
	* \brief Adds a icosphere, centered in (0, 0, 0)
	*
	* \param size Radius of the icosphere
	* \param recursionLevel Number of recursion for the icosphere
	* \param transformMatrix Matrix to apply
	*/

	void PrimitiveList::AddIcoSphere(float size, unsigned int recursionLevel, const Matrix4f& transformMatrix)
	{
		m_primitives.push_back(Primitive::IcoSphere(size, recursionLevel, transformMatrix));
	}

	/*!
	* \brief Adds a icosphere, centered in (0, 0, 0)
	*
	* \param size Radius of the sphere
	* \param recursionLevel Number of recursion for the icosphere
	* \param position Position of the icosphere
	* \param rotation Rotation of the icosphere
	*/

	void PrimitiveList::AddIcoSphere(float size, unsigned int recursionLevel, const Vector3f& position, const Quaternionf& rotation)
	{
		m_primitives.push_back(Primitive::IcoSphere(size, recursionLevel, position, rotation));
	}

	/*!
	* \brief Adds a plane, centered in (0, 0, 0)
	*
	* \param size (Width, Depth)
	* \param subdivision Number of subdivision for the axis
	* \param transformMatrix Matrix to apply
	*/

	void PrimitiveList::AddPlane(const Vector2f& size, const Vector2ui& subdivision, const Matrix4f& transformMatrix)
	{
		m_primitives.push_back(Primitive::Plane(size, subdivision, transformMatrix));
	}

	/*!
	* \brief Adds a plane, centered in (0, 0, 0)
	*
	* \param size (Width, Depth)
	* \param subdivision Number of subdivision for the axis
	* \param planeInfo Information for the plane
	*/

	void PrimitiveList::AddPlane(const Vector2f& size, const Vector2ui& subdivision, const Planef& planeInfo)
	{
		m_primitives.push_back(Primitive::Plane(size, subdivision, planeInfo));
	}

	/*!
	* \brief Adds a plane, centered in (0, 0, 0)
	*
	* \param size (Width, Depth)
	* \param subdivision Number of subdivision for the axis
	* \param position Position of the plane
	* \param rotation Rotation of the plane
	*/

	void PrimitiveList::AddPlane(const Vector2f& size, const Vector2ui& subdivision, const Vector3f& position, const Quaternionf& rotation)
	{
		m_primitives.push_back(Primitive::Plane(size, subdivision, position, rotation));
	}

	/*!
	* \brief Adds a UV sphere, centered in (0, 0, 0)
	*
	* \param size Radius of the sphere
	* \param sliceCount Number of slices
	* \param stackCount Number of stacks
	* \param transformMatrix Matrix to apply
	*/

	void PrimitiveList::AddUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Matrix4f& transformMatrix)
	{
		m_primitives.push_back(Primitive::UVSphere(size, sliceCount, stackCount, transformMatrix));
	}

	/*!
	* \brief Adds a UV sphere, centered in (0, 0, 0)
	*
	* \param size Radius of the sphere
	* \param sliceCount Number of slices
	* \param stackCount Number of stacks
	* \param position Position of the box
	* \param rotation Rotation of the box
	*/

	void PrimitiveList::AddUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Vector3f& position, const Quaternionf& rotation)
	{
		m_primitives.push_back(Primitive::UVSphere(size, sliceCount, stackCount, position, rotation));
	}

	/*!
	* \brief Gets the ith primitive
	* \return A reference to the ith primitive
	*
	* \param i Index of the primitive
	*
	* \remark Produces a NazaraAssert if index is greather than the size
	*/

	Primitive& PrimitiveList::GetPrimitive(std::size_t i)
	{
		NazaraAssert(i < m_primitives.size(), "Primitive index out of range");

		return m_primitives[i];
	}

	/*!
	* \brief Gets the ith primitive
	* \return A constant reference to the ith primitive
	*
	* \param i Index of the primitive
	*
	* \remark Produces a NazaraAssert if index is greather than the size
	*/

	const Primitive& PrimitiveList::GetPrimitive(std::size_t i) const
	{
		NazaraAssert(i < m_primitives.size(), "Primitive index out of range");

		return m_primitives[i];
	}

	/*!
	* \brief Gets the number of primitives
	* \return Number of primitives
	*/

	std::size_t PrimitiveList::GetSize() const
	{
		return m_primitives.size();
	}

	/*!
	* \brief Gets the ith primitive
	* \return A reference to the ith primitive
	*
	* \param i Index of the primitive
	*
	* \remark Produces a NazaraAssert if index is greather than the size
	*/

	Primitive& PrimitiveList::operator()(unsigned int i)
	{
		return GetPrimitive(i);
	}

	/*!
	* \brief Gets the ith primitive
	* \return A constant reference to the ith primitive
	*
	* \param i Index of the primitive
	*
	* \remark Produces a NazaraAssert if index is greather than the size
	*/

	const Primitive& PrimitiveList::operator()(unsigned int i) const
	{
		return GetPrimitive(i);
	}
}

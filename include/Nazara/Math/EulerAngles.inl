// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NazaraUtils/MathUtils.hpp>
#include <cstring>
#include <sstream>

namespace Nz
{

	/*!
	* \ingroup math
	* \class Nz::EulerAngles
	* \brief Math class that represents an Euler angle. Those describe a rotation transformation by rotating an object on its various axes in specified amounts per axis, and a specified axis order
	*
	* \remark Rotation are "right-handed", it means that you take your right hand, put your thumb finger in the direction you want and you other fingers represent the way of rotating
	*/

	/*!
	* \brief Constructs a EulerAngles object from its components
	*
	* \param P Pitch component = X axis
	* \param Y Yaw component = Y axis
	* \param R Roll component = Z axis
	*/
	template<typename T>
	constexpr EulerAngles<T>::EulerAngles(DegreeAngle<T> P, DegreeAngle<T> Y, DegreeAngle<T> R) :
	pitch(P),
	yaw(Y),
	roll(R)
	{
	}

	/*!
	* \brief Constructs a EulerAngles object from an array of three elements
	*
	* \param angles[3] angles[0] is pitch component, angles[1] is yaw component and angles[2] is roll component
	*/
	template<typename T>
	constexpr EulerAngles<T>::EulerAngles(const DegreeAngle<T> angles[3]) :
	EulerAngles(angles[0], angles[1], angles[2])
	{
	}

	/*!
	* \brief Constructs a EulerAngles object from an angle
	*
	* \param angle Angle representing a 2D rotation
	*/
	template<typename T>
	template<AngleUnit Unit>
	constexpr EulerAngles<T>::EulerAngles(const Angle<Unit, T>& angle) :
	EulerAngles(angle.ToEulerAngles())
	{
	}

	/*!
	* \brief Constructs a EulerAngles object from a quaternion
	*
	* \param quat Quaternion representing a rotation of space
	*/
	template<typename T>
	constexpr EulerAngles<T>::EulerAngles(const Quaternion<T>& quat) :
	EulerAngles(quat.ToEulerAngles())
	{
	}

	/*!
	* \brief Constructs a EulerAngles object from another type of EulerAngles
	*
	* \param angles EulerAngles of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	constexpr EulerAngles<T>::EulerAngles(const EulerAngles<U>& angles) :
	pitch(DegreeAngle<T>(angles.pitch)),
	yaw(DegreeAngle<T>(angles.yaw)),
	roll(DegreeAngle<T>(angles.roll))
	{
	}

	template<typename T>
	constexpr bool EulerAngles<T>::ApproxEqual(const EulerAngles& angles, T maxDifference) const
	{
		return pitch.ApproxEqual(angles.pitch, maxDifference) && yaw.ApproxEqual(angles.yaw, maxDifference) && roll.ApproxEqual(angles.roll, maxDifference);
	}

	/*!
	* \brief Normalizes the euler angle
	* \return A reference to this euler angle with has been normalized
	*
	* \see NormalizeAngle
	*/
	template<typename T>
	constexpr EulerAngles<T>& EulerAngles<T>::Normalize()
	{
		pitch.Normalize();
		yaw.Normalize();
		roll.Normalize();

		return *this;
	}

	/*!
	* \brief Converts the euler angle to quaternion
	* \return A Quaternion which represents the rotation of this euler angle
	*/
	template<typename T>
	Quaternion<T> EulerAngles<T>::ToQuaternion() const
	{
		// XYZ
		auto [s1, c1] = (yaw / T(2.0)).GetSinCos();
		auto [s2, c2] = (roll / T(2.0)).GetSinCos();
		auto [s3, c3] = (pitch / T(2.0)).GetSinCos();

		return Quaternion<T>(c1 * c2 * c3 - s1 * s2 * s3,
		                     s1 * s2 * c3 + c1 * c2 * s3,
		                     s1 * c2 * c3 + c1 * s2 * s3,
		                     c1 * s2 * c3 - s1 * c2 * s3);
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "EulerAngles(pitch, yaw, roll)"
	*/
	template<typename T>
	std::string EulerAngles<T>::ToString() const
	{
		std::ostringstream ss;
		ss << *this;

		return ss.str();
	}

	/*!
	* \brief Adds the components of the euler angle with other euler angle
	* \return A euler angle where components are the sum of this euler angle and the other one
	*
	* \param angles The other euler angle to add components with
	*/
	template<typename T>
	constexpr EulerAngles<T> EulerAngles<T>::operator+(const EulerAngles& angles) const
	{
		return EulerAngles(pitch + angles.pitch,
		                   yaw + angles.yaw,
		                   roll + angles.roll);
	}

	/*!
	* \brief Substracts the components of the euler angle with other euler angle
	* \return A euler angle where components are the difference of this euler angle and the other one
	*
	* \param angles The other euler angle to substract components with
	*/
	template<typename T>
	constexpr EulerAngles<T> EulerAngles<T>::operator-(const EulerAngles& angles) const
	{
		return EulerAngles(pitch - angles.pitch,
		                   yaw - angles.yaw,
		                   roll - angles.roll);
	}

	/*!
	* \brief Adds the components of other euler angle to this euler angle
	* \return A reference to this euler angle where components are the sum of this euler angle and the other one
	*
	* \param angles The other euler angle to add components with
	*/
	template<typename T>
	constexpr EulerAngles<T>& EulerAngles<T>::operator+=(const EulerAngles& angles)
	{
		pitch += angles.pitch;
		yaw += angles.yaw;
		roll += angles.roll;

		return *this;
	}

	/*!
	* \brief Substracts the components of other euler angle to this euler angle
	* \return A reference to this euler angle where components are the difference of this euler angle and the other one
	*
	* \param angles The other euler angle to substract components with
	*/
	template<typename T>
	constexpr EulerAngles<T>& EulerAngles<T>::operator-=(const EulerAngles& angles)
	{
		pitch -= angles.pitch;
		yaw -= angles.yaw;
		roll -= angles.roll;

		return *this;
	}

	/*!
	* \brief Compares the euler angle to other one
	* \return true if the euler angles are the same
	*
	* \param angles Other euler angle to compare with
	*/
	template<typename T>
	constexpr bool EulerAngles<T>::operator==(const EulerAngles& angles) const
	{
		return pitch == angles.pitch && yaw == angles.yaw && roll == angles.roll;
	}

	/*!
	* \brief Compares the euler angle to other one
	* \return false if the euler angles are the same
	*
	* \param angles Other euler angle to compare with
	*/
	template<typename T>
	constexpr bool EulerAngles<T>::operator!=(const EulerAngles& angles) const
	{
		return !operator==(angles);
	}

	template<typename T>
	constexpr bool EulerAngles<T>::operator<(const EulerAngles& angles) const
	{
		if (pitch != angles.pitch)
			return pitch < angles.pitch;

		if (yaw != angles.yaw)
			return yaw < angles.yaw;

		return roll < angles.roll;
	}

	template<typename T>
	constexpr bool EulerAngles<T>::operator<=(const EulerAngles& angles) const
	{
		if (pitch != angles.pitch)
			return pitch < angles.pitch;

		if (yaw != angles.yaw)
			return yaw < angles.yaw;

		return roll <= angles.roll;
	}

	template<typename T>
	constexpr bool EulerAngles<T>::operator>(const EulerAngles& angles) const
	{
		if (pitch != angles.pitch)
			return pitch > angles.pitch;

		if (yaw != angles.yaw)
			return yaw > angles.yaw;

		return roll > angles.roll;
	}

	template<typename T>
	constexpr bool EulerAngles<T>::operator>=(const EulerAngles& angles) const
	{
		if (pitch != angles.pitch)
			return pitch > angles.pitch;

		if (yaw != angles.yaw)
			return yaw > angles.yaw;

		return roll >= angles.roll;
	}

	template<typename T>
	constexpr bool EulerAngles<T>::ApproxEqual(const EulerAngles& lhs, const EulerAngles& rhs, T maxDifference)
	{
		return lhs.ApproxEqual(rhs, maxDifference);
	}

	/*!
	* \brief Shorthand for the euler angle (0, 0, 0)
	* \return A euler angle with components (0, 0, 0)
	*/
	template<typename T>
	constexpr EulerAngles<T> EulerAngles<T>::Zero()
	{
		return EulerAngles(0, 0, 0);
	}

	/*!
	* \brief Serializes a EulerAngles
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param angles Input euler angles
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const EulerAngles<T>& angles, TypeTag<EulerAngles<T>>)
	{
		if (!Serialize(context, angles.pitch))
			return false;

		if (!Serialize(context, angles.yaw))
			return false;

		if (!Serialize(context, angles.roll))
			return false;

		return true;
	}

	/*!
	* \brief Deserializes a EulerAngles
	* \return true if successfully deserialized
	*
	* \param context Serialization context
	* \param angles Output euler angles
	*/
	template<typename T>
	bool Deserialize(SerializationContext& context, EulerAngles<T>* angles, TypeTag<EulerAngles<T>>)
	{
		if (!Deserialize(context, &angles->pitch))
			return false;

		if (!Deserialize(context, &angles->yaw))
			return false;

		if (!Deserialize(context, &angles->roll))
			return false;

		return true;
	}

	/*!
	* \brief Output operator
	* \return The stream
	*
	* \param out The stream
	* \param angles The euler angle to output
	*/
	template<typename T>
	std::ostream& operator<<(std::ostream& out, const EulerAngles<T>& angles)
	{
		return out << "EulerAngles(" << angles.pitch << ", " << angles.yaw << ", " << angles.roll << ')';
	}
}

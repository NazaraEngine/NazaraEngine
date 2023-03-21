// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <NazaraUtils/Algorithm.hpp>
#include <memory>
#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	inline JPH::ShapeSettings* JoltCollider3D::GetShapeSettings() const
	{
		return m_shapeSettings.get();
	}

	template<typename T>
	const T* JoltCollider3D::GetShapeSettingsAs() const
	{
		return SafeCast<T*>(m_shapeSettings.get());
	}

	template<typename T>
	void JoltCollider3D::SetupShapeSettings(std::unique_ptr<T> shapeSettings)
	{
		assert(!m_shapeSettings);
		shapeSettings->SetEmbedded(); // Call SetEmbedded on the template type to prevent compiler to resolve it outside of a file including Jolt
		m_shapeSettings = std::move(shapeSettings);
	}


	inline JoltTranslatedRotatedCollider3D::JoltTranslatedRotatedCollider3D(std::shared_ptr<JoltCollider3D> collider, const Vector3f& translation) :
	JoltTranslatedRotatedCollider3D(std::move(collider), translation, Quaternionf::Identity())
	{
	}

	inline JoltTranslatedRotatedCollider3D::JoltTranslatedRotatedCollider3D(std::shared_ptr<JoltCollider3D> collider, const Quaternionf& rotation) :
	JoltTranslatedRotatedCollider3D(std::move(collider), Vector3f::Zero(), rotation)
	{
	}
}

#include <Nazara/JoltPhysics3D/DebugOff.hpp>

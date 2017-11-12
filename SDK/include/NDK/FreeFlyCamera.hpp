// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_FREEFLYCAMERA_HPP
#define NDK_FREEFLYCAMERA_HPP

#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/Utility/Node.hpp>
#include <NDK/EntityOwner.hpp>

namespace Ndk
{
	class FreeFlyCamera;

	using FreeFlyCameraHandle = Nz::ObjectHandle<FreeFlyCamera>;

	class NDK_API FreeFlyCamera : public Nz::Node, public Nz::HandledObject<FreeFlyCamera>
	{
		public:
			FreeFlyCamera(World& world, Nz::RenderWindow& window, float zfar = 5000.f, float znear = .1f);

			inline void EnableSmoothedMovement(bool smooth = true);
			
			inline float GetSensitivity() const;
			inline float GetSpeed() const;

			inline bool IsMovementSmoothed() const;

			inline void SetSensitivity(float sensivity);
			inline void SetSpeed(float speed);
			
		private:
			void OnKeyPressed(const Nz::EventHandler* handler, const Nz::WindowEvent::KeyEvent& e);
			void OnKeyReleased(const Nz::EventHandler* handler, const Nz::WindowEvent::KeyEvent& e);
			void OnMouseMoved(const Nz::EventHandler* handler, const Nz::WindowEvent::MouseMoveEvent& e);

			NazaraSlot(Nz::EventHandler, OnKeyPressed, m_onKeyPressed);
			NazaraSlot(Nz::EventHandler, OnKeyReleased, m_onKeyReleased);
			NazaraSlot(Nz::EventHandler, OnMouseMoved, m_onMouseMoved);
			
			EntityOwner m_camera;
			Nz::EulerAnglesf m_angles;
			Nz::RenderWindow& m_window;
			Nz::Vector3f m_targetPos;
			bool m_speedUp;
			float m_sensitivity;
			float m_speed;
	};
}

#include <NDK/FreeFlyCamera.inl>

#endif  // NDK_FREEFLYCAMERA_HPP
// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/FreeFlyCamera.hpp>
#include <NDK/Application.hpp>
#include <NDK/Components/CameraComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/World.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::FreeFlyCamera
	* \brief NDK class that represents a free fly camera to enable user to navigate freely in his scene
	*/

	/*!
	* \brief Constructs a FreeFlyCamera object in a specific world
	*
	* \param world World where the camera has to be created
	* \param window Window to target with the camera
	* \param zfar Distance between eyes and the farthest plan
	* \param znear Distance between eyes and the nearest plan (0 is a forbidden value because division by 0 is so)
	*/

	FreeFlyCamera::FreeFlyCamera(World& world, Nz::RenderWindow& window, float zfar, float znear) :
	m_angles(Nz::EulerAnglesf::Zero()),
	m_window(window),
	m_targetPos(Nz::Vector3f::Zero()),
	m_sensitivity(0.3f),
	m_speed(5.f)
	{
		Nz::EventHandler& eventHandler = m_window.GetEventHandler();
		m_onKeyPressed.Connect(eventHandler.OnKeyPressed, this, &FreeFlyCamera::OnKeyPressed);
		m_onKeyReleased.Connect(eventHandler.OnKeyReleased, this, &FreeFlyCamera::OnKeyReleased);
		m_onMouseMoved.Connect(eventHandler.OnMouseMoved, this, &FreeFlyCamera::OnMouseMoved);

		m_camera = world.CreateEntity();
		m_camera->AddComponent<Ndk::NodeComponent>().SetParent(this);
		m_camera->AddComponent<Ndk::VelocityComponent>().damped = true;

		Ndk::CameraComponent& cameraComp = m_camera->AddComponent<Ndk::CameraComponent>();
		cameraComp.SetZFar(zfar);
		cameraComp.SetZNear(znear);
		cameraComp.SetTarget(&m_window);

		SetRotation(m_angles);
	}

	/*!
	* \brief Handle action when a key is pressed
	*
	* Speed-up when Space is pressed
	* Go forward when Z or Up arrow is pressed
	* Go backward when S or Down arrow is pressed
	* Go left when Q or Left arrow is pressed
	* Go right when D or Right arrow is pressed
	* Go up when either right shift or left shift is pressed
	* Go down when either right control or left control is pressed
	*
	* \param handler [UNUSED]
	* \param e The structure that contains several informations about the key currently pressed (such as the key code)
	*/

	void FreeFlyCamera::OnKeyPressed(const Nz::EventHandler* /*handler*/, const Nz::WindowEvent::KeyEvent& e)
	{
		const float speed = m_speed * (m_speedUp ? 2.f : 1.f);
		Ndk::NodeComponent& node = m_camera->GetComponent<Ndk::NodeComponent>();
		Ndk::VelocityComponent& vel = m_camera->GetComponent<Ndk::VelocityComponent>();

		switch (e.code)
		{
			case Nz::Keyboard::Space:
				m_speedUp = true;
				break;
			
			case Nz::Keyboard::Z:
			case Nz::Keyboard::Up:
				vel = node.GetForward() * speed;
				break;

			case Nz::Keyboard::S:
			case Nz::Keyboard::Down:
				vel = node.GetBackward() * speed;
				break;

			case Nz::Keyboard::Q:
			case Nz::Keyboard::Left:
				vel = node.GetLeft() * speed;
				break;

			case Nz::Keyboard::D:
			case Nz::Keyboard::Right:
				vel = node.GetRight() * speed;
				break;

			case Nz::Keyboard::LShift:
			case Nz::Keyboard::RShift:
				vel = Nz::Vector3f::Up() * speed;
				break;
			
			case Nz::Keyboard::LControl:
			case Nz::Keyboard::RControl:
				vel = Nz::Vector3f::Down() * speed;
				break;

			default:
				break;
		}
	}

	/*!
	* \brief Handle action when a key is released
	*
	* Stop speed-up when Space is released
	* Stop any movement when one of the movement key is released
	*
	* \param handler [UNUSED]
	* \param e The structure that contains several informations about the key currently released (such as the key code)
	*/

	void FreeFlyCamera::OnKeyReleased(const Nz::EventHandler* /*handler*/, const Nz::WindowEvent::KeyEvent& e)
	{
		switch (e.code)
		{
			case Nz::Keyboard::Space:
				m_speedUp = false;
				break;
			
			case Nz::Keyboard::Z:
			case Nz::Keyboard::Up:
			case Nz::Keyboard::S:
			case Nz::Keyboard::Down:
			case Nz::Keyboard::Q:
			case Nz::Keyboard::Left:
			case Nz::Keyboard::D:
			case Nz::Keyboard::Right:
			case Nz::Keyboard::LShift:
			case Nz::Keyboard::RShift:
			case Nz::Keyboard::LControl:
			case Nz::Keyboard::RControl:
				m_camera->GetComponent<Ndk::VelocityComponent>().linearVelocity.MakeZero();
				break;

			default:
				break;
		}
	}

	/*!
	* \brief Handle camera rotation when the mouse is moved
	*
	* \param handler [UNUSED]
	* \param e The structure that contains several informations about the mouse movement (such as delta movement)
	*/

	void FreeFlyCamera::OnMouseMoved(const Nz::EventHandler* /*handler*/, const Nz::WindowEvent::MouseMoveEvent& e)
	{
		// We change the camera angle thanks to the X relative movement of the mouse
		m_angles.yaw = Nz::NormalizeAngle(m_angles.yaw - e.deltaX*m_sensitivity);

		// Same, but to avoid calculation problems of the view matrix, we clamp the angles
		m_angles.pitch = Nz::Clamp(m_angles.pitch - e.deltaY*m_sensitivity, -89.f, 89.f);

		// We apply Euler angles to our cam
		m_camera->GetComponent<Ndk::NodeComponent>().SetRotation(m_angles);

		// To avoid cursor to go outside the window, we recenter it
		// This function doesn't cause an event MouseMoved
		Nz::Vector2ui size = m_window.GetSize();
		Nz::Mouse::SetPosition(size.x / 2, size.y / 2, m_window); // TODO: Fix this on Linux (cc @Gawaboumbga)
	}
}
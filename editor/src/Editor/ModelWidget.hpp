#include <Nazara/Graphics/Model.hpp>
#include <NDK/World.hpp>
#include <NdkQt/QtCanvas.hpp>
#include <QtCore/QTimer>

#pragma once

class ModelWidget : public Ndk::QtCanvas
{
	Q_OBJECT

	public:
		ModelWidget(QWidget* parent = nullptr);
		~ModelWidget();

		void OnModelChanged(const Nz::ModelRef& model);

		void ResetCamera();

		void ShowNormals(bool normals = true);

	private:
		enum class MouseControl
		{
			Movement,
			None,
			Rotation
		};

		Nz::MeshRef CreateGridMesh(unsigned int size);
		void HandleEvent(const Nz::WindowEvent& event);
		void UpdateCamera();
		bool OnWindowCreated() override;
		void OnWindowResized() override;
		void Update();

		Ndk::EntityHandle m_camera;
		Ndk::EntityHandle m_modelEntity;
		Ndk::EntityHandle m_grid;
		Ndk::World m_world;
		Nz::Boxf m_box;
		Nz::Clock m_updateClock;
		Nz::EulerAnglesf m_cameraAngles;
		Nz::ModelRef m_gridModel;
		Nz::ModelRef m_model;
		Nz::ModelRef m_normalModel;
		QTimer m_updateTimer;
		MouseControl m_mouseControl;
		float m_cameraDistance;
};

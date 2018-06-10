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

		void EnableFreeflyCamera(bool freeflyCamera);

		void InvalidateNormals();

		void OnModelChanged(const Nz::ModelRef& model);

		void ResetCamera();

		void ShowAABB(bool showAABB = true);
		void ShowNormals(bool normals = true);
		void ShowSubmeshes(const Nz::Bitset<>& submeshes);

	private:
		enum class MouseControl
		{
			Movement,
			None,
			Camera
		};

		Nz::MeshRef CreateGridMesh(unsigned int size);
		void HandleEvent(const Nz::WindowEvent& event);
		bool OnWindowCreated() override;
		void OnWindowResized() override;
		void Update();
		void UpdateCamera();

		Ndk::EntityHandle m_camera;
		Ndk::EntityHandle m_modelEntity;
		Ndk::EntityHandle m_grid;
		Ndk::World m_world;
		Nz::Boxf m_box;
		Nz::Clock m_updateClock;
		Nz::EulerAnglesf m_cameraAngles;
		Nz::MaterialRef m_disabledMaterial;
		Nz::MaterialRef m_normalMaterial;
		Nz::ModelRef m_disabledModel;
		Nz::ModelRef m_gridModel;
		Nz::ModelRef m_model;
		Nz::ModelRef m_normalModel;
		Nz::Vector3f m_freeflyPosition;
		QTimer m_updateTimer;
		MouseControl m_mouseControl;
		bool m_freeflyCamera;
		float m_cameraDistance;
};

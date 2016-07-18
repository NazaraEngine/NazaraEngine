#include <Editor/MaterialEditor.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <QtCore/QObject>
#include <QtWidgets/QMainWindow>

#pragma once

class ModelWidget;
class QAction;
class QDoubleSpinBox;
class QDialog;
class QDockWidget;
class QFrame;
class QGridLayout;
class QListWidget;
class QListWidgetItem;
class QPoint;
class QPushButton;
class QTextEdit;

class EditorWindow : public QMainWindow
{
	Q_OBJECT

	public:
		EditorWindow(QWidget* parent = nullptr);
		~EditorWindow();

		void SetModel(Nz::ModelRef model);

		void ShowSubmeshes(const Nz::Bitset<>& submeshes);

	private:
		void ApplyTransform(const Nz::Matrix4f& transform);
		void BuildMenu();
		QPushButton* BuildTransformDialog(QDialog*& dialog, const QString& name, const QString& unitName, const QString& buttonName, std::size_t valueCount, QDoubleSpinBox** spinBoxes, const char* const* valueNames);
		void BuildTransformDialogs();
		void OnEditMaterial(std::size_t matIndex);
		void OnExport();
		void OnFlipUVs();
		void OnImport();
		void OnRegenerateNormals();
		void OnMaterialEdited(MaterialEditor* editor, std::size_t matIndex, const Nz::ParameterList& materialParameters);
		void OnMaterialSelected();
		void OnNormalToggled(bool active);
		void OnRecenter();
		void OnSubmeshSelected();
		void ShowMaterialContextMenu(const QPoint& location);
		void UpdateFaceFilling();

		std::array<QDoubleSpinBox*, 3> m_rotationValues;
		std::array<QDoubleSpinBox*, 3> m_scaleValues;
		std::array<QDoubleSpinBox*, 3> m_translateValues;
		Nz::Bitset<> m_activeSubmeshes;
		Nz::FaceFilling m_faceFilling;
		Nz::ModelRef m_model;
		QAction* m_showNormalButton;
		QDialog* m_rotateDialog;
		QDialog* m_scaleDialog;
		QDialog* m_translateDialog;
		QDockWidget* m_consoleDock;
		QDockWidget* m_materialsDock;
		QDockWidget* m_submeshesDock;
		QFrame* m_mainFrame;
		QGridLayout* m_mainLayout;
		QListWidget* m_materialList;
		QListWidget* m_subMeshList;
		QMetaObject::Connection m_subMeshListOnSelectionChange;
		QTextEdit* m_textEdit;
		MaterialEditor* m_materialEditor;
		ModelWidget* m_modelWidget;

		NazaraSlot(MaterialEditor, OnMaterialEditorSave, m_materialEditedSlot);
};

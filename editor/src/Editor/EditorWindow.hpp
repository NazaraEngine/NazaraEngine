#include <Editor/MaterialEditor.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <QtCore/QObject>
#include <QtWidgets/QMainWindow>

#pragma once

class ModelWidget;
class QAction;
class QDockWidget;
class QFrame;
class QGridLayout;
class QListWidget;
class QListWidgetItem;
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
		void BuildMenu();
		void OnEditMaterial(QListWidgetItem* item);
		void OnFlipUVs();
		void OnImport();
		void OnMaterialEdited(MaterialEditor* editor, std::size_t matIndex, const Nz::ParameterList& materialParameters);
		void OnMaterialSelected();
		void OnNormalToggled(bool active);
		void OnSubmeshSelected();
		void UpdateFaceFilling();

		Nz::Bitset<> m_activeSubmeshes;
		Nz::FaceFilling m_faceFilling;
		Nz::ModelRef m_model;
		QAction* m_showNormalButton;
		QDockWidget* m_consoleDock;
		QDockWidget* m_materialsDock;
		QDockWidget* m_submeshesDock;
		QFrame* m_mainFrame;
		QGridLayout* m_mainLayout;
		QListWidget* m_materialList;
		QListWidget* m_subMeshList;
		QTextEdit* m_textEdit;
		MaterialEditor* m_materialEditor;
		ModelWidget* m_modelWidget;

		NazaraSlot(MaterialEditor, OnMaterialEditorSave, m_materialEditedSlot);
};

#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <QtCore/QObject>
#include <QtWidgets/QMainWindow>

#pragma once

class ModelWidget;
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
		void OnFlipUVs();
		void OnImport();
		void OnNormalToggled(bool active);
		void OnSubmeshChanged();

		std::vector<Nz::MaterialRef> m_activesMaterials;
		std::vector<Nz::MaterialRef> m_disabledMaterials;
		Nz::Bitset<> m_activeSubmeshes;
		Nz::ModelRef m_model;
		QFrame* m_mainFrame;
		QGridLayout* m_mainLayout;
		QListWidget* m_subMeshList;
		QTextEdit* m_textEdit;
		ModelWidget* m_modelWidget;
};

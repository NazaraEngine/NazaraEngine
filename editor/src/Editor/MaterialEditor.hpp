#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtGui/QColor>
#include <QtWidgets/QDockWidget>
#include <array>
#include <unordered_set>

#pragma once

class QColorDialog;
class QLabel;
class QWidget;

class MaterialEditor : public QDockWidget
{
	Q_OBJECT

	public:
		MaterialEditor(QWidget* parent = nullptr);
		~MaterialEditor();

		void FillValues(std::size_t matIndex, Nz::ParameterList list);

		NazaraSignal(OnMaterialEditorSave, MaterialEditor* /*editor*/, std::size_t /*matIndex*/, const Nz::ParameterList& /*parameters*/);

	private:
		void OnColorSelected(const QColor& color);
		void OnFieldReset(std::size_t fieldIndex);
		void OnFieldUpdate(std::size_t fieldIndex);
		void OnUpdatePressed(bool checked);
		void PrepareColorPicker(std::size_t fieldIndex);

		enum class FieldType
		{
			Boolean,
			Color,
			FilePath,
			String
		};

		struct MaterialField
		{
			const char* group;
			const char* name;
			const char* key;
			FieldType type;
		};

		struct FieldData
		{
			const MaterialField* matField;
			QLabel* label;
			QWidget* widget;
		};

		QColorDialog* m_colorDialog;
		Nz::Bitset<> m_changedFields;
		Nz::ParameterList m_parameters;
		std::size_t m_currentField;
		std::size_t m_materialIndex;
		std::vector<FieldData> m_fields;
		bool m_fieldLocked;

		static std::array<MaterialField, 20> s_fields;
};

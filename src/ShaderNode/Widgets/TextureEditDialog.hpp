#pragma once

#ifndef NAZARA_SHADERNODES_TEXTUREEDITDIALOG_HPP
#define NAZARA_SHADERNODES_TEXTUREEDITDIALOG_HPP

#include <ShaderNode/Enums.hpp>
#include <QtWidgets/QDialog>

class QComboBox;
class QLineEdit;
class QSpinBox;

struct TextureInfo
{
	std::size_t bindingIndex;
	std::size_t setIndex;
	std::string name;
	TextureType type;
};

class TextureEditDialog : public QDialog
{
	public:
		TextureEditDialog(QWidget* parent = nullptr);
		TextureEditDialog(const TextureInfo& Texture, QWidget* parent = nullptr);
		~TextureEditDialog() = default;

		TextureInfo GetTextureInfo() const;

	private:
		void OnAccept();

		QComboBox* m_typeList;
		QLineEdit* m_textureName;
		QSpinBox* m_bindingIndex;
		QSpinBox* m_setIndex;
};

#include <ShaderNode/Widgets/TextureEditDialog.inl>

#endif

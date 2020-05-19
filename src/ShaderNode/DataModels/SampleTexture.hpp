#pragma once

#ifndef NAZARA_SHADERNODES_SAMPLETEXTURE_HPP
#define NAZARA_SHADERNODES_SAMPLETEXTURE_HPP

#include <QtWidgets/QComboBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <ShaderGraph.hpp>
#include <DataModels/ShaderNode.hpp>
#include <array>

class SampleTexture : public ShaderNode
{
	public:
		SampleTexture(ShaderGraph& graph);
		~SampleTexture() = default;

		Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* /*expressions*/, std::size_t count) const override;

		QString caption() const override { return "Sample texture"; }
		QString name() const override { return "SampleTexture"; }

		QWidget* embeddedWidget() override;
		unsigned int nPorts(QtNodes::PortType portType) const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

	protected:
		void UpdatePreview();
		void UpdateTextureList();

		void ComputePreview(QPixmap& pixmap) const;

		NazaraSlot(ShaderGraph, OnTextureListUpdate, m_onTextureListUpdate);

		QComboBox* m_textureSelection;
		QLabel* m_pixmapLabel;
		QPixmap m_pixmap;
		QWidget* m_widget;
		QVBoxLayout* m_layout;
};

#include <DataModels/SampleTexture.inl>

#endif

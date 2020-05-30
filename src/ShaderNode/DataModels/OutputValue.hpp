#pragma once

#ifndef NAZARA_SHADERNODES_FRAGMENTOUTPUT_HPP
#define NAZARA_SHADERNODES_FRAGMENTOUTPUT_HPP

#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>

class QFormLayout;

class OutputValue : public ShaderNode
{
	public:
		OutputValue(ShaderGraph& graph);

		void BuildNodeEdition(QFormLayout* layout) override;

		Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const override;

		QString caption() const override { return "Output"; }
		QString name() const override { return "Output"; }

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		unsigned int nPorts(QtNodes::PortType portType) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void setInData(std::shared_ptr<QtNodes::NodeData> value, int index) override;

	private:
		bool ComputePreview(QPixmap& pixmap) override;
		void OnOutputListUpdate();

		NazaraSlot(ShaderGraph, OnOutputListUpdate, m_onOutputListUpdateSlot);
		NazaraSlot(ShaderGraph, OnOutputUpdate, m_onOutputUpdateSlot);

		std::optional<std::size_t> m_currentOutputIndex;
		std::shared_ptr<VecData> m_input;
		std::string m_currentOutputText;
};

#include <ShaderNode/DataModels/OutputValue.inl>

#endif

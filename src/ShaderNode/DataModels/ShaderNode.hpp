#pragma once

#ifndef NAZARA_SHADERNODES_SHADERNODE_HPP
#define NAZARA_SHADERNODES_SHADERNODE_HPP

#include <Nazara/Math/Vector2.hpp>
#include <NZSL/Ast/Nodes.hpp>
#include <nodes/NodeDataModel>
#include <QtGui/QPixmap>
#include <optional>

class QLabel;
class QFormLayout;
class ShaderGraph;

class ShaderNode : public QtNodes::NodeDataModel
{
	public:
		ShaderNode(ShaderGraph& graph);

		virtual nzsl::Ast::NodePtr BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const = 0;
		virtual void BuildNodeEdition(QFormLayout* layout);

		inline void DisablePreview();
		void EnablePreview(bool enable = true);

		inline ShaderGraph& GetGraph();
		inline const ShaderGraph& GetGraph() const;
		virtual int GetOutputOrder() const;
		inline const std::string& GetVariableName() const;

		inline void SetPreviewSize(const Nz::Vector2i& size);
		inline void SetVariableName(std::string variableName);

		QWidget* embeddedWidget() final;

		void restore(const QJsonObject& data) override;
		QJsonObject save() const override;

		void setInData(std::shared_ptr<QtNodes::NodeData>, int) override;

	protected:
		inline void DisableCustomVariableName();
		inline void EnableCustomVariableName(bool enable = true);
		virtual QWidget* EmbeddedWidget();
		void UpdatePreview();

	private:
		virtual bool ComputePreview(QPixmap& pixmap);

		Nz::Vector2i m_previewSize;
		QLabel* m_pixmapLabel;
		QWidget* m_embeddedWidget;
		std::optional<QPixmap> m_pixmap;
		std::string m_variableName;
		ShaderGraph& m_graph;
		bool m_enableCustomVariableName;
		bool m_isPreviewEnabled;
};

#include <ShaderNode/DataModels/ShaderNode.inl>

#endif

#pragma once

#ifndef NAZARA_SHADERNODES_SHADERNODE_HPP
#define NAZARA_SHADERNODES_SHADERNODE_HPP

#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/ShaderAst.hpp>
#include <nodes/NodeDataModel>
#include <QtGui/QPixmap>
#include <optional>

class QLabel;
class QVBoxLayout;
class ShaderGraph;

class ShaderNode : public QtNodes::NodeDataModel
{
	public:
		ShaderNode(ShaderGraph& graph);

		virtual void BuildNodeEdition(QVBoxLayout* layout);

		void EnablePreview(bool enable);

		virtual Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const = 0;
		inline ShaderGraph& GetGraph();
		inline const ShaderGraph& GetGraph() const;

		void SetPreviewSize(const Nz::Vector2i& size);

		QWidget* embeddedWidget() final;

		void setInData(std::shared_ptr<QtNodes::NodeData>, int) override;

	protected:
		void UpdatePreview();

	private:
		virtual bool ComputePreview(QPixmap& pixmap);

		Nz::Vector2i m_previewSize;
		QLabel* m_pixmapLabel;
		std::optional<QPixmap> m_pixmap;
		ShaderGraph& m_graph;
		bool m_isPreviewEnabled;
};

#include <ShaderNode/DataModels/ShaderNode.inl>

#endif

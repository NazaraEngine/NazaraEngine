#include <ShaderNode/Widgets/NodeEditor.hpp>
#include <QtWidgets/QLabel>

template<typename F>
void NodeEditor::UpdateContent(QString nodeName, F&& callback)
{
	Clear();

	m_layout = new QVBoxLayout;
	setLayout(m_layout);

	QLabel* label = new QLabel(nodeName);
	m_layout->addWidget(label);

	callback(m_layout);
}

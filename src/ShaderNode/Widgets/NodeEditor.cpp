#include <ShaderNode/Widgets/NodeEditor.hpp>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

NodeEditor::NodeEditor() :
m_layout(nullptr)
{
}

void NodeEditor::Clear()
{
	if (m_layout)
	{
		while (QWidget* w = findChild<QWidget*>())
			delete w;

		delete m_layout;
		m_layout = nullptr;
	}
}

#include <QtCore/QObject>
#include <QtWidgets/QMainWindow>

#pragma once

class QFrame;

class EditorWindow : public QMainWindow
{
	Q_OBJECT

	public:
		EditorWindow();
		~EditorWindow();

	private:
		QFrame* m_mainFrame;
};

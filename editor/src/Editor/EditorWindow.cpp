#include <Editor/EditorWindow.hpp>
#include <QtWidgets>

EditorWindow::EditorWindow()
{
	QMenu* menuFichier = menuBar()->addMenu("&Fichier");
	QMenu* fichiersRecents = menuFichier->addMenu("Fichiers &récents");
	fichiersRecents->addAction("Fichier bidon 1.txt");
	fichiersRecents->addAction("Fichier bidon 2.txt");
	fichiersRecents->addAction("Fichier bidon 3.txt");

	QMenu* menuEdition = menuBar()->addMenu("&Edition");
	QMenu* menuAffichage = menuBar()->addMenu("&Affichage");

	setWindowTitle("Nazara Model Importer");
}


EditorWindow::~EditorWindow()
{
}

#include <Editor/EditorWindow_moc.inl>
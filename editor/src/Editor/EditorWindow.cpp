#include <Editor/EditorWindow.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Utility.hpp>
#include <Editor/ModelWidget.hpp>
#include <QtWidgets>
#include <iostream>

EditorWindow::EditorWindow(QWidget* parent) :
QMainWindow(parent)
{
	m_disabledModel = Nz::Model::New();
	m_disabledMaterial = Nz::Material::New();
	m_disabledMaterial->Enable(Nz::RendererParameter_Blend, true);
	m_disabledMaterial->Enable(Nz::RendererParameter_DepthWrite, false);
	m_disabledMaterial->Enable(Nz::RendererParameter_FaceCulling, false);
	m_disabledMaterial->SetDiffuseColor(Nz::Color(128, 128, 128, 50));
	m_disabledMaterial->SetDstBlend(Nz::BlendFunc_InvSrcAlpha);
	m_disabledMaterial->SetSrcBlend(Nz::BlendFunc_SrcAlpha);


	QMenu* menuFichier = menuBar()->addMenu("&Fichier");
	QAction* import = menuFichier->addAction("Importer...");
	connect(import, &QAction::triggered, this, &EditorWindow::OnImport);


	QMenu* fichiersRecents = menuFichier->addMenu("Fichiers &récents");
	fichiersRecents->addAction("Fichier bidon 1.txt");
	fichiersRecents->addAction("Fichier bidon 2.txt");
	fichiersRecents->addAction("Fichier bidon 3.txt");

	QMenu* menuEdition = menuBar()->addMenu("&Edition");
	QAction* flipUVs = menuEdition->addAction("Inverser les coordonnées de texture");
	connect(flipUVs, &QAction::triggered, this, &EditorWindow::OnFlipUVs);

	QMenu* drawMenu = menuBar()->addMenu("&Affichage");

	QAction* drawNormals = new QAction("Draw normals", drawMenu);
	drawNormals->setCheckable(true);

	drawMenu->addAction(drawNormals);

	QWidget* mainWidget = new QWidget;

	m_modelWidget = new ModelWidget;

	m_textEdit = new QTextEdit;
	m_textEdit->setReadOnly(true);

	m_subMeshList = new QListWidget;
	m_subMeshList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_subMeshList->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));

	connect(m_subMeshList, &QListWidget::itemSelectionChanged, this, &EditorWindow::OnSubmeshChanged);

	m_mainLayout = new QGridLayout;
	m_mainLayout->addWidget(m_modelWidget, 0, 0, 3, 3);
	m_mainLayout->addWidget(m_subMeshList, 0, 3, 1, 1);
	m_mainLayout->addWidget(m_textEdit,    3, 0, 1, 1);

	mainWidget->setLayout(m_mainLayout);

	setCentralWidget(mainWidget);

	statusBar()->showMessage("Prêt");
}

EditorWindow::~EditorWindow()
{
}

void EditorWindow::SetModel(Nz::ModelRef model)
{
	m_model = std::move(model);
	m_modelWidget->OnModelChanged(m_model, m_disabledModel);

	Nz::Mesh* mesh = m_model->GetMesh();

	m_subMeshList->clear();
	m_subMeshList->addItem("All submeshes");

	std::size_t subMeshCount = mesh->GetSubMeshCount();
	for (std::size_t i = 0; i < mesh->GetSubMeshCount(); ++i)
	{
		Nz::String name = "Submesh #" + Nz::String::Number(i);

		QListWidgetItem* item = new QListWidgetItem(name.GetConstBuffer());
		item->setData(Qt::UserRole, i);

		m_subMeshList->addItem(item);
	}

	m_disabledModel->SetMesh(mesh);
	for (std::size_t i = 0; i < mesh->GetMaterialCount(); ++i)
		m_disabledModel->SetMaterial(i, m_disabledMaterial);

	OnSubmeshChanged();
}

void EditorWindow::ShowSubmeshes(const Nz::Bitset<>& submeshes)
{
	m_activeSubmeshes = submeshes;

	m_model->ShowSubmeshes(submeshes);
	m_disabledModel->ShowSubmeshes(~submeshes);
}

void EditorWindow::OnFlipUVs()
{
	if (m_model)
	{
		Nz::Mesh* mesh = m_model->GetMesh();
		std::size_t subMeshCount = mesh->GetSubMeshCount();

		for (std::size_t i = 0; i < subMeshCount; ++i)
		{
			if (!m_activeSubmeshes.Test(i))
				continue;

			Nz::SubMesh* submesh = mesh->GetSubMesh(i);
			Nz::VertexMapper mapper(submesh);
			std::size_t vertexCount = submesh->GetVertexCount();

			Nz::SparsePtr<Nz::Vector2f> texCoords = mapper.GetComponentPtr<Nz::Vector2f>(Nz::VertexComponent_TexCoord);

			for (unsigned int j = 0; j < vertexCount; ++j)
			{
				texCoords->Set(texCoords->x, 1.0f - texCoords->y);
				++texCoords;
			}
		}
	}
}

void EditorWindow::OnImport()
{
	QString filePath = QFileDialog::getOpenFileName(this, "Import a model");
	if (filePath.isEmpty())
		return;

	Nz::ErrorFlags errFlags(Nz::ErrorFlag_Silent | Nz::ErrorFlag_ThrowException, true);

	Nz::ModelRef model = Nz::Model::New();

	Nz::Clock loadClock;

	try
	{
		Nz::ModelParameters parameters;
		parameters.mesh.optimizeIndexBuffers = false;

		model->LoadFromFile(filePath.toUtf8().constData(), parameters);
	}
	catch (const std::exception& e)
	{
		Nz::String message = "Failed to load model: ";
		message += e.what();

		m_textEdit->append(message.GetConstBuffer());

		return;
	}

	SetModel(std::move(model));

	statusBar()->showMessage("Loaded " + filePath + " in " + QString::number(loadClock.GetSeconds()) + "s");
}

void EditorWindow::OnNormalToggled(bool active)
{

}

void EditorWindow::OnSubmeshChanged()
{
	auto selectedItems = m_subMeshList->selectedItems();

	Nz::Bitset<> activeSubmeshes(m_model->GetMesh()->GetSubMeshCount(), false);
	if (selectedItems.isEmpty())
		activeSubmeshes.Set(true);
	else
	{
		for (QListWidgetItem* item : selectedItems)
		{
			QVariant data = item->data(Qt::UserRole);
			if (data.isNull())
				activeSubmeshes.Set(true);
			else
				activeSubmeshes.Set(data.toInt(), true);
		}
	}

	ShowSubmeshes(activeSubmeshes);
}

#include <Editor/EditorWindow_moc.inl>
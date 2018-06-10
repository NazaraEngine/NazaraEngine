#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Utility/Joint.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/Sequence.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <cctype>
#include <iostream>
#include <limits>

int main()
{
	// Pour charger des ressources, il est impératif d'initialiser le module utilitaire
	Nz::Initializer<Nz::Utility> utility;
	if (!utility)
	{
		// Ça n'a pas fonctionné, le pourquoi se trouve dans le fichier NazaraLog.log
		std::cout << "Failed to initialize Nazara, see NazaraLog.log for further informations" << std::endl;
		std::getchar(); // On laise le temps de voir l'erreur
		return EXIT_FAILURE;
	}

	for (;;)
	{
		Nz::Directory resourceDirectory("resources");
		if (!resourceDirectory.Open())
		{
			std::cerr << "Failed to open resource directory" << std::endl;
			std::getchar();
			return EXIT_FAILURE;
		}

		std::vector<Nz::String> models;
		while (resourceDirectory.NextResult())
		{
			Nz::String path = resourceDirectory.GetResultName();
			Nz::String ext = path.SubStringFrom('.', -1, true); // Tout ce qui vient après le dernier '.' de la chaîne
			if (Nz::MeshLoader::IsExtensionSupported(ext)) // L'extension est-elle supportée par le MeshLoader ?
				models.push_back(path);
		}

		resourceDirectory.Close();

		if (models.empty())
		{
			std::cout << "No loadable mesh found in resource directory" << std::endl;
			std::getchar();
			return EXIT_FAILURE;
		}

		std::cout << "Choose:" << std::endl;
		std::cout << "0: exit" << std::endl;
		for (unsigned int i = 0; i < models.size(); ++i)
			std::cout << (i+1) << ": " << models[i] << std::endl;

		std::cout << std::endl;

		unsigned int iChoice;
		do
		{
			std::cout << '-';
			std::cin >> iChoice;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		while (iChoice > models.size());

		if (iChoice == 0)
			break;

		Nz::Mesh mesh;
		if (!mesh.LoadFromFile(resourceDirectory.GetPath() + '/' + models[iChoice-1]))
		{
			std::cout << "Failed to load mesh" << std::endl;
			std::getchar();
			return EXIT_FAILURE;
		}

		switch (mesh.GetAnimationType())
		{
			case Nz::AnimationType_Skeletal:
				std::cout << "This is a skeletal-animated mesh" << std::endl;
				break;

			case Nz::AnimationType_Static:
				std::cout << "This is a static mesh" << std::endl;
				break;

			// Inutile de faire un case default (GetAnimationType renverra toujours une valeur correcte)
		}

		std::cout << "It has a total of " << mesh.GetVertexCount() << " vertices for " << mesh.GetSubMeshCount() << " submesh(es)." << std::endl;

		if (mesh.IsAnimable())
		{
			if (mesh.GetAnimationType() == Nz::AnimationType_Skeletal)
			{
				const Nz::Skeleton* skeleton = mesh.GetSkeleton();
				unsigned int jointCount = skeleton->GetJointCount();
				std::cout << "It has a skeleton made of " << skeleton->GetJointCount() << " joint(s)." << std::endl;
				std::cout << "Print joints ? (Y/N) ";

				char cChoice;
				std::cin >> cChoice;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				if (std::tolower(cChoice) == 'y')
				{
					for (unsigned int i = 0; i < jointCount; ++i)
					{
						const Nz::Joint* joint = skeleton->GetJoint(i);
						std::cout << "\t" << (i+1) << ": " << joint->GetName();

						const Nz::Joint* parent = static_cast<const Nz::Joint*>(joint->GetParent());
						if (parent)
							std::cout << " (Parent: " << parent->GetName() << ')';

						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					}
				}
			}

			Nz::String animationPath = mesh.GetAnimation();
			if (!animationPath.IsEmpty())
			{
				Nz::Animation animation;
				if (animation.LoadFromFile(animationPath))
				{
					unsigned int sequenceCount = animation.GetSequenceCount();
					std::cout << "It has an animation made of " << animation.GetFrameCount() << " frame(s) for " << sequenceCount << " sequence(s)." << std::endl;
					std::cout << "Print sequences ? (Y/N) ";

					char cChoice;
					std::cin >> cChoice;
					std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

					if (std::tolower(cChoice) == 'y')
					{
						for (unsigned int i = 0; i < sequenceCount; ++i)
						{
							const Nz::Sequence* sequence = animation.GetSequence(i);
							std::cout << "\t" << (i+1) << ": " << sequence->name << std::endl;
							std::cout << "\t\tStart frame: " << sequence->firstFrame << std::endl;
							std::cout << "\t\tFrame count: " << sequence->frameCount << std::endl;
							std::cout << "\t\tFrame rate: " << sequence->frameRate << std::endl;
							std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						}
					}
				}
				else
					std::cout << "It has animation information but animation file could'nt be loaded" << std::endl;
			}
			else
				std::cout << "It's animable but has no animation information" << std::endl;
		}

		Nz::Boxf cube = mesh.GetAABB();
		std::cout << "Mesh is " << cube.width << " units wide, " << cube.height << " units height and " << cube.depth << " units depth" << std::endl;

		unsigned int materialCount = mesh.GetMaterialCount();
		std::cout << "It has " << materialCount << " materials registred" << std::endl;
		std::cout << "Print materials ? (Y/N) ";

		char cChoice;
		std::cin >> cChoice;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (std::tolower(cChoice) == 'y')
		{
			for (unsigned int i = 0; i < materialCount; ++i)
			{
				const Nz::ParameterList& matData = mesh.GetMaterialData(i);

				Nz::String data;
				if (!matData.GetStringParameter(Nz::MaterialData::FilePath, &data))
					data = "<Custom>";

				std::cout << "\t" << (i+1) << ": " << data << std::endl;
			}
		}

		std::cout << std::endl << std::endl;
	}

	// Le module utilitaire et le mesh sont déchargés automatiquement
	return EXIT_SUCCESS;
}

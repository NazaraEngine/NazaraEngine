#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Math/Cube.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <iostream>
#include <limits>

int main()
{
	// Pour charger des ressources, il est impératif d'initialiser le module utilitaire
	NzInitializer<NzUtility> utility;
	if (!utility)
	{
		// Ça n'a pas fonctionné, le pourquoi se trouve dans le fichier NazaraLog.log
		std::cout << "Failed to initialize Nazara, see NazaraLog.log for further informations" << std::endl;
		std::getchar(); // On laise le temps de voir l'erreur
		return EXIT_FAILURE;
	}

	for (;;)
	{
		NzDirectory resourceDirectory("resources/");
		if (!resourceDirectory.Open())
		{
			std::cerr << "Failed to open resource directory" << std::endl;
			std::getchar();
			return EXIT_FAILURE;
		}

		std::vector<NzString> models;
		while (resourceDirectory.NextResult(true))
		{
			NzString path = resourceDirectory.GetResultName();
			if (path.EndsWith(".md2") || path.EndsWith(".md5mesh"))
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

		NzMesh mesh;
		if (!mesh.LoadFromFile("resources/" + models[iChoice-1]))
		{
			std::cout << "Failed to load mesh" << std::endl;
			std::getchar();
			return EXIT_FAILURE;
		}

		switch (mesh.GetAnimationType())
		{
			case nzAnimationType_Keyframe:
				std::cout << "This is a keyframe-animated mesh" << std::endl;
				break;

			case nzAnimationType_Skeletal:
				std::cout << "This is a skeletal-animated mesh" << std::endl;
				break;

			case nzAnimationType_Static:
				std::cout << "This is a static mesh" << std::endl;
				break;

			// Inutile de faire un case default (GetAnimationType renverra toujours une valeur correcte)
		}

		std::cout << "It has a total of " << mesh.GetVertexCount() << " vertices for " << mesh.GetSubMeshCount() << " submesh(es)." << std::endl;

		if (mesh.IsAnimable())
		{
			if (mesh.GetAnimationType() == nzAnimationType_Skeletal)
			{
				const NzSkeleton* skeleton = mesh.GetSkeleton();
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
						const NzJoint* joint = skeleton->GetJoint(i);
						std::cout << "\t" << (i+1) << ": " << joint->GetName();

						const NzJoint* parent = static_cast<const NzJoint*>(joint->GetParent());
						if (parent)
							std::cout << " (Parent: " << parent->GetName() << ')';

						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					}
				}
			}

			if (mesh.HasAnimation())
			{
				const NzAnimation* animation = mesh.GetAnimation();
				unsigned int sequenceCount = animation->GetSequenceCount();
				std::cout << "It has an animation made of " << animation->GetFrameCount() << " frame(s) for " << sequenceCount << " sequence(s)." << std::endl;
				std::cout << "Print sequences ? (Y/N) ";

				char cChoice;
				std::cin >> cChoice;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				if (std::tolower(cChoice) == 'y')
				{
					for (unsigned int i = 0; i < sequenceCount; ++i)
					{
						const NzSequence* sequence = animation->GetSequence(i);
						std::cout << "\t" << (i+1) << ": " << sequence->name << std::endl;
						std::cout << "\t\tStart frame: " << sequence->firstFrame << std::endl;
						std::cout << "\t\tFrame count: " << sequence->frameCount << std::endl;
						std::cout << "\t\tFrame rate: " << sequence->frameRate << std::endl;
						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					}
				}
			}
			else
				std::cout << "It's animable but has no loaded animation" << std::endl;
		}

		/*NzCubef cube = mesh.GetAABB().GetCube();
		std::cout << "Mesh is " << cube.width << " unit wide, " << cube.height << "unit height and " << cube.depth << " unit depth" << std::endl;

		unsigned int materialCount = mesh.GetMaterialCount();
		std::cout << "It has " << materialCount << " materials registred" << std::endl;
		for (unsigned int i = 0; i < materialCount; ++i)
			std::cout << "\t" << (i+1) << ": " << mesh.GetMaterial(i) << std::endl;*/

		std::cout << std::endl << std::endl;
	}

	// Le module utilitaire et le mesh sont déchargés automatiquement
	return EXIT_SUCCESS;
}

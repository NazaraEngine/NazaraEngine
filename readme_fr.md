Platforme | Build Status | Nightlies
------------ | ------------- | -------------
Windows | [![AppVeyor Build status](https://ci.appveyor.com/api/projects/status/dj5qx7axym4uakmy/branch/master?svg=true)](https://ci.appveyor.com/project/DPSLynix/nazaraengine/branch/master) | MSVC14:  [x86](https://ci.appveyor.com/api/projects/DPSLynix/NazaraEngine/artifacts/package%2FNazaraEngine.7z?branch=master&job=Environment%3A%20TOOLSET%3Dvs2015%3B%20Configuration%3A%20ReleaseDynamic%3B%20Platform%3A%20Win32)   [x86_64](https://ci.appveyor.com/api/projects/DPSLynix/NazaraEngine/artifacts/package%2FNazaraEngine.7z?branch=master&job=Environment%3A%20TOOLSET%3Dvs2015%3B%20Configuration%3A%20ReleaseDynamic%3B%20Platform%3A%20x64)
Linux | [![Travis CI Build Status](https://travis-ci.org/DigitalPulseSoftware/NazaraEngine.svg?branch=master)](https://travis-ci.org/DigitalPulseSoftware/NazaraEngine) | Non

# Nazara Engine  

Nazara Engine est une API rapide, complète, portable et orientée-objet qui peut vous aider dans votre vie de développeur de tous les jours.  
Son objectif est de fournir un ensemble de classes utiles : Son noyau dispose de chaînes de caractères unicode, de gestion du système de fichiers, hashs, threads, ...

Il propose aussi un ensemble de bibliothèques, comme audio, réseau, physique, moteur de rendu, moteur 2D et 3D, ...

Vous pouvez également l'utiliser pour toute application commerciale sans aucune contrainte ([Licence MIT](http://opensource.org/licenses/MIT)).

## Auteurs

Jérôme "Lynix" Leclercq - développeur principal (<lynix680@gmail.com>)  
Rémi "overdrivr" Bèges - développeur & aide - module Noise - (<remi.beges@laposte.net>)

## Installation

Utilisez le système premake pour construire le projet du moteur, dans le dossier build, pour ensuite compiler le moteur pour votre plateforme.

## Utilisation

Vous pouvez lire des tutoriaux sur l'installation, la compilation et l'utilisation sur le [wiki officiel](https://github.com/DigitalPulseSoftware/NazaraEngine/wiki) (**\*En cours de rédaction***)

## Contribution

##### N'hésitez pas à contribuer à Nazara Engine en :
- Contribuant au [wiki](https://github.com/DigitalPulseSoftware/NazaraEngine/wiki) (**\*Lien brisé***)  
- Soumettant un patch sur GitHub  
- Postant des suggestions/bugs sur le forum ou sur le [tracker GitHub](https://github.com/DigitalPulseSoftware/NazaraEngine/issues)  
- Faisant un [fork du projet](https://github.com/DigitalPulseSoftware/NazaraEngine/fork) sur GitHub et en [proposant vos changements](https://github.com/DigitalPulseSoftware/NazaraEngine/pulls)  
- Parlant du Nazara Engine à d'autres personnes  
- Faisant n'importe quoi d'autre qui pourrait nous aider  

## Liens

[Website](https://nazara.digitalpulsesoftware.net)  
[Documentation](https://nazara.digitalpulsesoftware.net/doc)  
[Mattermost](https://mattermost.digitalpulsesoftware.net)  
[Wiki](https://github.com/DigitalPulseSoftware/NazaraEngine/wiki)  
[Forum](https://forum.digitalpulsesoftware.net)  

###Remerciements:

- **RafBill** et **Raakz:** Recherche de bugs et/ou tests  
- **Fissal "DrFisher" Hannoun**: Aide et conseils lors de la conception de l'architecture du moteur  
- **Alexandre "Danman" Janniaux**: Aide sur l'implémentation POSIX
- **Youri "Gawaboumga" Hubaut**: Amélioration du moteur tant au niveau du code que de sa documentation et du projet en général.

#include <Client/Graphics/BitmapText.hpp>
#include <Client/System/GameDirectoryGuesser.hpp>
#include <Client/System/NEApplication.hpp>
#include <Client/System/NEInterface.hpp>
#include <SFML/Graphics/Image.hpp>
#include <Shared/Utils/FileChooser.hpp>
#include <Shared/Utils/MessageBox.hpp>
#include <Shared/Utils/OSDetect.hpp>
#include <Shared/Utils/Utilities.hpp>
#include <cstdbool>
#include <exception>
#include <iostream>

#ifdef WOS_WINDOWS
#	include <Windows.h>
#	undef MessageBox
#endif

std::string NEApplication::getGameDirectory() const
{
	return gameDirectory;
}

std::string NEApplication::getResourceDirectory() const
{
	return resourceDirectory;
}

bool NEApplication::checkGameDirectory() const
{
	return fileExists(gameDirectory + "/data/necrodancer.xml");
}

bool NEApplication::chooseGameDirectory()
{
	// First, check if game data is in command-line supplied directory or current directory (if no argument was given).
	if (checkGameDirectory())
	{
		return true;
	}

	// Check common game directories.
	for (auto dir : GameDirectoryGuesser::getDirectoryList())
	{
		gameDirectory = dir;

		if (checkGameDirectory())
		{
			return true;
		}
	}

	bool showMessage = false;

	FileChooser fc;
	fc.setTitle("Select NecroDancer directory");

	MessageBox errorMessage("This is not a valid NecroDancer directory.\n\n"
		"Please select your NecroDancer installation directory (typically found in "
		"your SteamApps folder)\nor click \"Cancel\" to exit.", "NecroEdit", MessageBox::Error, MessageBox::OkCancel);

	do
	{
		if (showMessage)
		{
			if (!errorMessage.show())
			{
				return false;
			}
		}

		// Let the user choose the game data directory.
		fc.showDialog(FileChooser::OpenFolder);
		fc.wait();
		if (fc.isDone())
		{
			gameDirectory = fc.getSelectedFile();
		}
		else
		{
			return false;
		}

		showMessage = true;

	}
	while (!checkGameDirectory());

	return true;
}

int NEApplication::init(const std::vector<std::string>& args)
{
#ifdef WOS_WINDOWS
	FreeConsole();
#endif

	resourceDirectory = ".";
	gameDirectory = ".";

	enum ArgMode
	{
		ArgNone,
		ArgGameDirectory,
		ArgResourceDirectory,
		ArgDungeon
	};

	ArgMode argMode = ArgNone;

	for (const std::string & arg : args)
	{
		switch (argMode)
		{
		case ArgNone:

			if (arg == "-g" || arg == "--game-directory")
			{
				argMode = ArgGameDirectory;
				break;
			}

			if (arg == "-r" || arg == "--resource-directory")
			{
				argMode = ArgResourceDirectory;
				break;
			}

			if (arg == "--")
			{
				argMode = ArgDungeon;
				break;
			}

			if (arg == "-h" || arg == "-?" || arg == "--help")
			{
				printUsage(args[0]);
				return LoadErrorArgs;
			}

			// Fall through to dungeon file specification.

		case ArgDungeon:
			// Only allow specifying one dungeon file.
			if (dungeonFile.empty())
			{
				dungeonFile = arg;
			}
			else
			{
				printUsage(args[0]);
				return LoadErrorArgs;
			}
			break;

		case ArgGameDirectory:
			gameDirectory = arg;
			argMode = ArgNone;
			break;

		case ArgResourceDirectory:
			resourceDirectory = arg;
			argMode = ArgNone;
			break;

		default:
			break;
		}
	}

	if (argMode != ArgNone)
	{
		printUsage(args[0]);
		return 1;
	}

	if (resourceDirectory.empty() || resourceDirectory.back() != '/')
	{
		resourceDirectory.push_back('/');
	}

	if (!chooseGameDirectory())
	{
		return LoadErrorNecroAssets;
	}

	try
	{
		setFont(FontDefault, loadFont(resourceDirectory + "necroedit-font.png"));
		setFont(FontSymbolSmall, loadFont(resourceDirectory + "necroedit-symbol.png"));
		setFont(FontSymbolLarge, loadFont(resourceDirectory + "necroedit-symbol-large.png", 32));
	}
	catch (std::exception & ex)
	{
		MessageBox fontError(ex.what(), "NecroEdit", MessageBox::Error, MessageBox::Ok);
		fontError.show();
		return LoadErrorFont;
	}

	setTextureSmoothingEnabled(false);

	open();

	return LoadSuccess;
}

void NEApplication::printUsage(const std::string & exeName)
{
	std::cout << "Usage: " << exeName << " [Options] [Dungeon File]" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "-g / --game-directory [dir]: Specify [dir] as containing the game's executable and resource files"
		<< std::endl;
	std::cout << "-r / --resource-directory [dir]: Specify [dir] as containing the editor's resource files"
		<< std::endl;
	std::cout << "-h / --help: Show this help" << std::endl;
	std::cout << "--: Treat following arguments as dungeon file to load, rather than as options" << std::endl;
}

bool NEApplication::onLoadImage(const std::string& imageIdentifier, sf::Image& image)
{
	return image.loadFromFile(imageIdentifier);
}

std::shared_ptr<gui2::Interface> NEApplication::makeInterface()
{
	return std::make_shared<NEInterface>();
}

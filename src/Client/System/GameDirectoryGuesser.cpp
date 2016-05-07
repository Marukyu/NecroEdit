#include <Client/System/GameDirectoryGuesser.hpp>
#include <Shared/Utils/OSDetect.hpp>

#ifdef WOS_WINDOWS
#	include <windows.h>
#	include <shlobj.h>
#elif defined WOS_LINUX || defined WOS_OSX
#	include <pwd.h>
#	include <unistd.h>
#	include <cstdlib>
#endif

std::vector<std::string> GameDirectoryGuesser::getDirectoryList()
{
	std::vector<std::string> directories;

#ifdef WOS_WINDOWS

	// TODO: Handle non-ASCII paths better.
	TCHAR programFilesDirectory[MAX_PATH];
	TCHAR programFilesX86Directory[MAX_PATH];
	SHGetFolderPath(nullptr, CSIDL_PROGRAM_FILES, nullptr, 0, programFilesDirectory);
	SHGetFolderPath(nullptr, CSIDL_PROGRAM_FILESX86, nullptr, 0, programFilesX86Directory);

	std::basic_string<TCHAR> programFilesW(programFilesDirectory);
	std::basic_string<TCHAR> programFilesX86W(programFilesX86Directory);

	std::string programFilesA(programFilesW.begin(), programFilesW.end());
	std::string programFilesX86A(programFilesX86W.begin(), programFilesX86W.end());

	directories.push_back(programFilesA + "\\Steam\\SteamApps\\common\\Crypt of the NecroDancer");
	directories.push_back(programFilesX86A + "\\Steam\\SteamApps\\common\\Crypt of the NecroDancer");

#elif defined WOS_LINUX

	// Get user's home directory, if set through $HOME.
	const char * homeDirectoryPtr = std::getenv("HOME");

	if (homeDirectoryPtr == nullptr)
	{
		// Get user's home directory, if not set through $HOME.
		homeDirectoryPtr = getpwuid(getuid())->pw_dir;
	}

	if (homeDirectoryPtr != nullptr)
	{
		std::string homeDirectory(homeDirectoryPtr);

		// Try all capitalization variants. The exact capitalization depends on when Steam was installed/cleaned.
		directories.push_back(homeDirectory + "/.local/share/steam/steamapps/common/Crypt of the NecroDancer");
		directories.push_back(homeDirectory + "/.local/share/Steam/steamapps/common/Crypt of the NecroDancer");
		directories.push_back(homeDirectory + "/.local/share/steam/SteamApps/common/Crypt of the NecroDancer");
		directories.push_back(homeDirectory + "/.local/share/Steam/SteamApps/common/Crypt of the NecroDancer");
	}

#elif defined WOS_OSX

	// Get user's home directory, if set through $HOME.
	const char * homeDirectoryPtr = std::getenv("HOME");

	if (homeDirectoryPtr == nullptr)
	{
		// Get user's home directory, if not set through $HOME.
		homeDirectoryPtr = getpwuid(getuid())->pw_dir;
	}

	if (homeDirectoryPtr != nullptr)
	{
		std::string homeDirectory(homeDirectoryPtr);

		directories.push_back(homeDirectory + "/Library/Application Support/Steam/steamapps/common/Crypt of the NecroDancer/NecroDancer.app/Contents/Resources");
		// Default GOG install
		directories.push_back("/Applications/Crypt of the Necrodancer.app/Contents/Resources/game/NecroDancer.app/Contents/Resources");
	}

#endif


	return directories;
}

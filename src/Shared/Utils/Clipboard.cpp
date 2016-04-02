#include "Shared/Utils/Clipboard.hpp"
#include "Shared/Utils/OSDetect.hpp"

#ifdef WOS_LINUX

#include <cstdio>
#include <cstdlib>

std::string getClipboardText()
{
	std::FILE * clipFile;
	std::string text;

	clipFile = popen("xclip -selection clipboard -o", "r");

	if (clipFile == 0)
	{
		return "";
	}

	char readBuf[2048];
	int readBytes = 0;

	while ((readBytes = std::fread(readBuf, 1, sizeof(readBuf), clipFile)) != 0)
	{
		text.append(readBuf, readBytes);
	}

	pclose(clipFile);

	return text;
}

void setClipboardText(const std::string & text)
{
	std::FILE * clipFile;

	clipFile = popen("xclip -selection clipboard -i", "w");

	if (clipFile == 0)
	{
		return;
	}

	std::fwrite(text.c_str(), 1, text.size(), clipFile);

	pclose(clipFile);
}

#elif defined WOS_WINDOWS

#include <windows.h>
#include <cstring>

std::string getClipboardText()
{
	HANDLE clip = 0;

	if (OpenClipboard(0))
		clip = GetClipboardData(CF_TEXT);

	if (!clip)
		return "";

	return std::string((char*)clip);
}


void setClipboardText(const std::string & text)
{
	std::size_t len = text.size() + 1;

	HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, len);
	std::memcpy(GlobalLock(mem), text.c_str(), len);
	GlobalUnlock(mem);

	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, mem);
	CloseClipboard();
}


#else

namespace {

	std::string clipboard;

}

std::string getClipboardText()
{
	return clipboard;
}

void setClipboardText(const std::string & text)
{
	clipboard = text;
}

#endif

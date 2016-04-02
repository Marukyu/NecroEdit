#include <Shared/External/TinyFileDialogs/tinyfiledialogs.h>
#include <Shared/Utils/FileChooser.hpp>
#include <Shared/Utils/Utilities.hpp>
#include <cstdbool>

FileChooser::FileChooser() :
autoSetDefault(true),
cleared(false)
{
}

FileChooser::~FileChooser()
{
	if (dialogThread.joinable())
	{
		dialogThread.detach();
	}
}

void FileChooser::setTitle(std::string title)
{
	settings.title = title;
}

const std::string& FileChooser::getTitle() const
{
	return settings.title;
}

void FileChooser::setDefaultPath(std::string defaultPath)
{
	settings.defaultPath = defaultPath;
}

const std::string& FileChooser::getDefaultPath() const
{
	return settings.defaultPath;
}

void FileChooser::setFilterDescription(std::string filterDescription)
{
	settings.filterDescription = filterDescription;
}

const std::string& FileChooser::getFilterDescription() const
{
	return settings.filterDescription;
}

void FileChooser::setFilterPatterns(std::vector<std::string> filterPatterns)
{
	settings.filterPatterns = filterPatterns;
}

const std::vector<std::string>& FileChooser::getFilterPatterns() const
{
	return settings.filterPatterns;
}

void FileChooser::setMultiSelectAllowed(bool multiSelect)
{
	settings.multiSelect = multiSelect;
}

bool FileChooser::isMultiSelectAllowed() const
{
	return settings.multiSelect;
}

void FileChooser::setAutoSetDefault(bool autoSetDefault)
{
	this->autoSetDefault = autoSetDefault;
}

bool FileChooser::isAutoSetDefault() const
{
	return this->autoSetDefault;
}

bool FileChooser::showDialog(DialogType type)
{
	if (isOpen())
	{
		return false;
	}
	
	cleared = false;

	if (autoSetDefault && result && !result->selectedFiles.empty())
	{
		settings.defaultPath = result->selectedFiles.back();
	}

	result = std::make_shared<Result>();
	
	if (dialogThread.joinable())
	{
		dialogThread.detach();
	}

	dialogThread = std::thread(&FileChooser::execute, type, settings, result);

	return true;
}

void FileChooser::wait()
{
	if (isOpen())
	{
		dialogThread.join();
	}
}

bool FileChooser::isOpen() const
{
	return result && !result->done;
}

bool FileChooser::isDone() const
{
	return !cleared && result && result->done && !result->selectedFiles.empty();
}

bool FileChooser::isCancelled() const
{
	return !cleared && result && result->done && result->selectedFiles.empty();
}

void FileChooser::clear()
{
	if (!isOpen())
	{
		cleared = true;
	}
}

const std::string& FileChooser::getSelectedFile() const
{
	if (!isDone() || cleared)
	{
		static std::string empty;
		return empty;
	}

	return result->selectedFiles.back();
}

const std::vector<std::string>& FileChooser::getSelectedFiles() const
{
	if (!isDone() || cleared)
	{
		static std::vector<std::string> empty;
		return empty;
	}

	return result->selectedFiles;
}

void FileChooser::execute(DialogType type, FileChooserSettings settings, std::shared_ptr<Result> result)
{
	std::vector<const char *> filters;

	for (const std::string & filter : settings.filterPatterns)
	{
		filters.push_back(filter.c_str());
	}

	const char * ret = nullptr;

	switch (type)
	{
	case OpenFile:
		ret = tinyfd_openFileDialog(settings.title.c_str(), settings.defaultPath.c_str(), filters.size(),
			filters.data(), settings.filterDescription.c_str(), settings.multiSelect);
		break;

	case SaveFile:
		ret = tinyfd_saveFileDialog(settings.title.c_str(), settings.defaultPath.c_str(), filters.size(),
			filters.data(), settings.filterDescription.c_str());
		break;
		
	case OpenFolder:
		ret = tinyfd_selectFolderDialog(settings.title.c_str(), settings.defaultPath.c_str());
		break;
	}
	
	if (ret != nullptr)
	{
		if (settings.multiSelect)
		{
			// Assuming that the returned string is UTF-8, this should work.
			result->selectedFiles = splitString(ret, "|");
		}
		else
		{
			result->selectedFiles.push_back(ret);
		}
	}
	
	result->done = true;
}

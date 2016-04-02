#ifndef SRC_SHARED_UTILS_FILECHOOSER_HPP_
#define SRC_SHARED_UTILS_FILECHOOSER_HPP_

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

class FileChooser
{
public:

	enum DialogType
	{
		OpenFile,
		SaveFile,
		OpenFolder
	};

	FileChooser();
	~FileChooser();
	
	FileChooser(FileChooser && other) = default;
	FileChooser & operator=(FileChooser && other) = default;

	void setTitle(std::string title);
	const std::string & getTitle() const;

	void setDefaultPath(std::string defaultPath);
	const std::string & getDefaultPath() const;

	void setFilterDescription(std::string filterDescription);
	const std::string & getFilterDescription() const;

	void setFilterPatterns(std::vector<std::string> filterPatterns);
	const std::vector<std::string> & getFilterPatterns() const;

	void setMultiSelectAllowed(bool multiSelect);
	bool isMultiSelectAllowed() const;

	void setAutoSetDefault(bool autoSetDefault);
	bool isAutoSetDefault() const;

	bool showDialog(DialogType type);

	void wait();

	bool isOpen() const;
	bool isDone() const;
	bool isCancelled() const;
	
	void clear();

	const std::string & getSelectedFile() const;
	const std::vector<std::string> & getSelectedFiles() const;

private:

	struct FileChooserSettings
	{
		std::string title;
		std::string defaultPath;
		std::string filterDescription;
		std::vector<std::string> filterPatterns;
		bool multiSelect = false;
	};

	struct Result
	{
		std::atomic_bool done;
		std::vector<std::string> selectedFiles;
	};

	static void execute(DialogType type, FileChooserSettings settings, std::shared_ptr<Result> result);

	FileChooserSettings settings;
	bool autoSetDefault;
	bool cleared;

	std::thread dialogThread;
	std::shared_ptr<Result> result;
};

#endif

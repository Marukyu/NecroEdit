#ifndef NE_APPLICATION_HPP
#define NE_APPLICATION_HPP

#include <Client/GUI2/Application.hpp>
#include <memory>
#include <string>
#include <vector>

namespace sf
{
class Image;
}

class NEApplication : public gui2::Application
{
public:
	
	std::string getGameDirectory() const;
	std::string getResourceDirectory() const;
	
private:

	enum LoadReturnCodes
	{
		LoadSuccess = 0,
		LoadErrorArgs,
		LoadErrorFont,
		LoadErrorAssetFile,
		LoadErrorNecroAssets,
	};
	
	int init(const std::vector<std::string> & args) override;
	
	void printUsage(const std::string & exeName);
	
	bool checkGameDirectory() const;
	bool chooseGameDirectory();
	
	bool onLoadImage(const std::string & imageIdentifier, sf::Image & image) override;
	
	std::shared_ptr<gui2::Interface> makeInterface() override;
	
	std::string gameDirectory;
	std::string resourceDirectory;
	std::string dungeonFile;
};

#endif

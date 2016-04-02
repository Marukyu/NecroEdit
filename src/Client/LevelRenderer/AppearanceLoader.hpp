/*
 * AppearanceLoader.hpp
 *
 *  Created on: Nov 22, 2015
 *      Author: marukyu
 */

#ifndef SRC_CLIENT_LEVELRENDERER_APPEARANCELOADER_HPP_
#define SRC_CLIENT_LEVELRENDERER_APPEARANCELOADER_HPP_

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace sf
{
class Image;
}

class AppearanceLoader
{
public:
	
	struct Appearance
	{
		int id;
		unsigned int variant;
		std::string name;
		std::unique_ptr<sf::Image> image;
		bool isClipped;
		std::vector<std::string> custom;
		
		bool isDuplicate;
		int duplicateID;
	};

	typedef std::function<void(Appearance)> CallbackFunction;

	/**
	 * Loads object appearance information and calls the specified function for each object variant.
	 * 
	 * Object properties are taken to be in the following order: filename:x:y:width:height:<custom properties>...
	 * 
	 * Variants named "null" are skipped. All properties except filename are optional.
	 * 
	 * filename is the basePath-relative name of the image file containing the object variant's image. ".png" is
	 * automatically appended to it.
	 * 
	 * x, y, width and height specify the sub-rectangle of the image file.
	 * 
	 * Further properties are custom and will be passed to the callback function.
	 * 
	 * @param data
	 *        A newline-separated list of objects as comma-separated object variants with colon-separated properties.
	 * 
	 * @param section
	 *        The section (marked with brackets and the section name) to read the object data from.
	 * 
	 * @param basePath
	 *        Points to the directory containing the relevant image files. It is expected to end with a slash.
	 *        If basePath is empty, this function will not load images and ignore variant infos.
	 * 
	 * @param callback
	 *        The function to be called for every object variant.
	 */
	bool loadAppearance(const std::string & data, const std::string & section, const std::string & basePath,
			CallbackFunction callback);
};

#endif

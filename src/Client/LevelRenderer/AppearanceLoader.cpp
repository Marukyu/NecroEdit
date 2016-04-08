/*
 * AppearanceLoader.cpp
 *
 *  Created on: Nov 22, 2015
 *      Author: marukyu
 */

#include <Client/LevelRenderer/AppearanceLoader.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <Shared/Utils/StrNumCon.hpp>
#include <Shared/Utils/Utilities.hpp>
#include <Shared/Utils/MakeUnique.hpp>
#include <cstddef>
#include <iterator>

bool AppearanceLoader::loadAppearance(const std::string& data, const std::string& section, const std::string& basePath,
		CallbackFunction callback)
{
	// Parse list of objects (one per line).
	std::vector<std::string> lines = splitString(data, "\n");

	// Extract desired subsection.
	lines = extractSection(lines, section);

	// Process objects.
	for (const std::string & currentLineString : lines)
	{
		// Initialize variant ID.
		std::size_t variantID = 0;

		// Split comma-separated line data.
		std::vector<std::string> currentLine = splitString(currentLineString, ",");

		// Ignore invalid objects.
		if (currentLine.size() < 3)
		{
			continue;
		}

		// Read object variants.
		for (std::size_t i = 2; i < currentLine.size(); ++i)
		{
			// Create appearance object.
			Appearance appearance;

			// Read object ID and name.
			appearance.id = cStoI(currentLine[0]);
			appearance.name = currentLine[1];

			// Set variant count.
			appearance.variant = variantID;

			// Split colon-separated variant data.
			std::vector<std::string> currentVariant = splitString(currentLine[i], ":");

			// Ignore invalid or null variants.
			if (currentVariant.empty() || currentVariant[0] == "null")
			{
				continue;
			}

			// Check for duplicate backreference.
			if (currentVariant[0][0] == '=')
			{
				// Mark this appearance variant as a duplicate with the specified ID.
				appearance.isDuplicate = true;
				appearance.duplicateID = cStoI(currentVariant[0].substr(1));
				appearance.isClipped = false;
			}
			else
			{
				// Mark this appearance variant as a duplicate with the specified ID.
				appearance.isDuplicate = false;
				
				// Create image for current object.
				std::unique_ptr<sf::Image> currentImage = makeUnique<sf::Image>();

				// Load image for the object variant.
				if (!currentImage->loadFromFile(basePath + currentVariant[0] + ".png"))
				{
					continue;
				}

				// Create variable for image cropping.
				sf::IntRect clipRect;

				// Check if cropping information is specified.
				if (currentVariant.size() >= 5)
				{
					// Specify clipping flag.
					appearance.isClipped = true;

					// Read cropping rectangle.
					clipRect = sf::IntRect(cStoI(currentVariant[1]), cStoI(currentVariant[2]), cStoI(currentVariant[3]),
						cStoI(currentVariant[4]));

					// Copy custom properties.
					appearance.custom.assign(currentVariant.begin() + 5, currentVariant.end());

					// Create image for cropped object.
					std::unique_ptr<sf::Image> currentImageCropped = makeUnique<sf::Image>();

					// Crop the object image.
					currentImageCropped->create(clipRect.width, clipRect.height, sf::Color::Black);
					currentImageCropped->copy(*currentImage, 0, 0, clipRect);

					// Pass cropped image to appearance object.
					appearance.image = std::move(currentImageCropped);
				}
				else
				{
					// Specify clipping flag.
					appearance.isClipped = false;

					// Pass uncropped image to appearance object.
					appearance.image = std::move(currentImage);
				}
			}

			// Call callback function.
			callback(std::move(appearance));

			// Increment variant count.
			variantID++;
		}
	}

	return true;
}

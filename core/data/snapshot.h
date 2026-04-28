#pragma once

#include "image.h"
#include <string>

struct Snapshot {
	std::string stateFilename;
	std::string thumbFilename;
	Image image;

	std::string getStateFilename() { return stateFilename; }
	std::string getThumbfilename() { return thumbFilename; }
	Image getImage() { return image; }
};
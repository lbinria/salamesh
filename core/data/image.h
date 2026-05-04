#pragma once

struct Image {
	unsigned int texId;
	int width, height, channels;
	unsigned int getTexId() { return texId; }
	int getWidth() { return width; }
	int getHeight() { return height; }
	int getChannels() { return channels; }
};
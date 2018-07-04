#pragma once
#include "WindowView.h"

#include <fstream>

class RenderPipeline
{
public:
	void create(WindowView& windowViewRef) {
		createGraphicsPipeline();
	};
	void cleanup() {
		
	};
private:
	void createGraphicsPipeline() {
		
		auto vertShaderByteCode = readFile("test.txt");
		//auto vertShaderByteCode = readFile("shaders/vert.spv");
		std::cout << "Vertex shader buffer size: " << vertShaderByteCode.size() << std::endl;
		//auto fragShaderByteCode = readFile("shaders/frag.spv");
		//auto fragShaderByteCode = readFile("shaders/frag.spv");
		//std::cout << "Fragment shader buffer size: " << fragShaderByteCode.size() << std::endl;
	}

	static std::vector<char> readFile(const std::string& filename) {

		// open file
		// ate: "at the end" of the file; so we can estimate size easily
		// binary: read as binary file, no formatting and special characters
		std::ifstream testStream(filename);
		if (testStream.good()) {
			std::cout << "WOOHOO file found!" << std::endl;
		}
		else {
			std::cout << "file not found!" << std::endl;
		}
		testStream.close();

		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		// create buffer
		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		// read file into buffer
		file.seekg(0); // go to beginning
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}
};


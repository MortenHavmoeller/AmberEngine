#include "stdafx.h"
#include "filetool.h"

std::vector<char> filetool::readFile(const std::string& filename) {

	std::cout << "opening file " << filename << " ... ";

	// open file
	// ate: "at the end" of the file; so we can estimate size easily
	// binary: read as binary file, no formatting and special characters
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.good()) {
		std::cout << "file not good: ";

		if (file.fail()) {
			std::cout << "fail ";
		}

		if (file.bad()) {
			std::cout << "bad ";
		}

		if (file.eof()) {
			std::cout << "end-of-file ";
		}

		std::cout << std::endl;
	}
	else {
		std::cout << "file good" << std::endl;
	}

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

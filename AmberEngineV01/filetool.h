#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

class filetool
{
public:
	static std::vector<char> readFile(const std::string& filename);
};


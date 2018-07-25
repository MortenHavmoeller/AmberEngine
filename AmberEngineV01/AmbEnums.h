#pragma once
#include <string>

enum AmbAppType {
	APPLICATION_TYPE_UNDEFINED = 0,
	APPLICATION_TYPE_CLIENT = 1,
	APPLICATION_TYPE_SERVER = 2,
	APPLICATION_TYPE_SERVER_AND_CLIENT = 3 // 1 + 2
};

class AmbEnums {
public:
	static std::string getAmbAppTypeName(AmbAppType type) {
		switch (type) {
		case 0: return "APPLICATION_TYPE_UNDEFINED";
		case 1: return "APPLICATION_TYPE_CLIENT";
		case 2: return "APPLICATION_TYPE_SERVER";
		case 3: return "APPLICATION_TYPE_SERVER_AND_CLIENT";
		}
	}
};
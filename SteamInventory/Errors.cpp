#include <cstdlib>
#include "Errors.h"


	void fatalError(std::string n) {
		std::cout << n << std::endl;
		std::cout << "Press ENTER to exit." << std::endl;
		std::cin.ignore();
		getchar();
		exit(1);
	}

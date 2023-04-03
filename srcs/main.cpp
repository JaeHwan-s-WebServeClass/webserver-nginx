#include "./include/include.hpp"

int	main(int argc, char **argv) {
	if (argc == 2) {
		parser(static_cast<std::string>(argv[1]));
	}
	if (argc > 2) {
		errorHandler("argc is not 2");
	}
	// argc < 2인 경우에는 default config
}
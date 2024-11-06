#include "../common_lib.h"

void PrintError(const char* function, int errorCode) {
	if (DEBUG_MODE) {
		fprintf(stderr, "%s() failed (%d)\n", function, errorCode);
	}
	exit(errorCode);
}

void PrintMessage(const char* msg) {
	if (DEBUG_MODE) {
		printf("%s\n", msg);
	}
	return;	
}


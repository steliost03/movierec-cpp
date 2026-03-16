#include "csvparsing.h"

void throwErrorAndTerminate(const char* message){

	printf("- - -\n");
	printf("An error occured while parsing csv:\n");
    printf("ERROR : ");
	printf("%s",message);
	printf("- - -\n");
	printf("\nThe program will now terminate.");
	printf("\n");
	printf("Press Enter to continue.");
	getchar();
	exit(0);

}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
	char* buff;
	char body[200] = "The query string is: ";
	int length;

	if ((buff = getenv("QUERY_STRING")) != NULL) {
		strcat(body, buff);

		length = strlen(body);

		printf("Content-length: %d\n", length);
		printf("Content-type: text/plain\n");
		printf("%s\n", body);
		fflush(stdout);
	}
}

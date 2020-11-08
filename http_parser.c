#include<stdio.h>
#include<string.h>

#define HEADER_NAME_MAX_SIZE 512
#define HEADER_VALUE_MAX_SIZE 1024
#define MAX_HEADERS 32

typedef struct {
	char name[HEADER_NAME_MAX_SIZE];
	char value[HEADER_VALUE_MAX_SIZE];
} http_header;

/* Input: request, a string
 * Output: 1 if request is a complete HTTP request, 0 otherwise
 * */
int is_complete_request(const char *request) {
	int line_count = 0;
	char copy[1000];
	char* line;
	char* segment;

       	strcpy(copy, request);

	line = strtok(copy, "\n");
	while (line != NULL) {
		line_count += 1;
		line = strtok(NULL, "\n");
	}

	if (line_count < 2){
		return 0;
	}

	// checking first line for GET or POST.
	line = strtok(copy, "\n");
	segment = strtok(line, " ");

	if (strcmp(segment, "GET") != 0 && strcmp(segment, "POST") != 0) {
		return 0;
	}

	segment = strtok(NULL, " ");
	if (segment == NULL) {
		return 0;
	}

	segment = strtok(NULL, " ");
	if (segment == NULL) {
		return 0;
	}

	// resetting copy
	strcpy(copy, request);
	line = strtok(copy, "\n");

	for (int i = 1; i < line_count; i++) {
		line = strtok(NULL, "\n");
	}	

	if (strcmp(line, "\r") != 0) {
		return 0;
	}
	
	return 1;
}

/* Parse an HTTP request, and copy each parsed value into the
 * corresponding array as a NULL-terminated string.
 * Input: request - string containing the original request;
 *        should not be modifed.
 * Input: method, hostname, port, uri - arrays to which the
 *        corresponding parts parsed from the request should be
 *        copied, as strings.  The uri is the "file" part of the requested URL.
 *        If no port is specified, the port array should be populated with a
 *        string specifying the default HTTP port.
 * Input: headers - an array of http_headers, each of which should be
 *        populated with the corresponding name/value of a header.
 * Output: return the number of headers in the request.
 * */
int parse_request(const char *request, char *method,
		char *hostname, char *port, char *uri, http_header *headers) {

	char copy[1000];
	char* line;
	char* segment;
	char request_uri[500];
	int header_count = 0;

	strcpy(copy, request);

	// Parsing line 1
	line = strtok(copy, "\n");

	segment = strtok(line, " ");
	strcpy(method, segment);

	segment = strtok(NULL, " ");
	strcpy(request_uri, segment);

	//resetting copy. strtok really mutilates what it's working on, especially when called multiple times
	strcpy(copy, request);

	// Parsing line 2
	line = strtok(copy, "\n");
	line = strtok(NULL, "\n");

	segment = strtok(line, " ");
	segment = strtok(NULL, " ");
	strcpy(uri, request_uri + strlen(segment) + 7);

	segment = strtok(segment, ":");
	strcpy(hostname, segment);

	segment = strtok(NULL, ":");
	if (segment != NULL) {
		strcpy(port, segment);
	}
	else {
		strcpy(port, "80");
	}

	//resetting copy
	strcpy(copy, request);
	line = strtok(copy, "\n");
	line = strtok(NULL, "\n");

	// Parsing headers
	while (line != NULL) {
		segment = strtok(line, ":");

		if (segment != NULL && strcmp(segment, "\r") != 0) {
			strcpy(headers[header_count].name, segment);

			segment = strtok(NULL, ":");
			strcpy(headers[header_count].value, segment);

			header_count += 1;

			// resetting copy
			strcpy(copy, request);
			line = strtok(copy, "\n");

			for (int i = 0; i <= header_count; i++) {
				line = strtok(NULL, "\n");
			}
		}

		else {
			line = NULL;
		}
	}

	return header_count;
}

/* Iterate through an array of headers, and return the value
 * (as a char *) corresponding to the name passed.  If there is no
 * header with the name passed, return NULL.
 * Input: name - the name of the header whose value is being sought.
 * Input: headers - the array of http_headers to be searched.
 * Input: num_headers - the number of headers in the headers array.
 * */
char *get_header_value(const char *name, http_header *headers, int num_headers) {
	for (int i = 0; i < num_headers; i++) {
		if (strcmp(headers[i].name, name) == 0) {
			return headers[i].value;
		}
	}

	return NULL;
}


int main() {
	char s1[4096];
       	sprintf(s1, "GET http://www.example.com/index.html HTTP/1.0\r\n");
       	sprintf(s1, "%sHost: www.example.com\r\n", s1);
	sprintf(s1, "%sUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:68.0) Gecko/20100101 Firefox/68.0\r\n", s1);
	sprintf(s1, "%sAccept-Language: en-US,en;q=0.5\r\n\r\n", s1);

	char s2[4096];
       	sprintf(s2, "GET http://www.example.com:8080/index.html?foo=1&bar=2 HTTP/1.0\r\n");
       	sprintf(s2, "%sHost: www.example.com:8080\r\n", s2);
	sprintf(s2, "%sUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:68.0) Gecko/20100101 Firefox/68.0\r\n", s2);
	sprintf(s2, "%sAccept-Language: en-US,en;q=0.5\r\n\r\n", s2);

	char s3[4096];
       	sprintf(s3, "GET http://www.example.com:8080/index.html HTTP/1.0\r\n");
	
	char method[50];
	char hostname[500];
	char port[50];
	char uri[500];
	int num_headers;
	http_header headers[MAX_HEADERS];

	if (is_complete_request(s1)) {
		num_headers = parse_request(s1, method, hostname, port, uri, headers);
		printf("s1 method: %s\n", method);
		printf("s1 hostname: %s\n", hostname);
		printf("s1 port: %s\n", port);
		printf("s1 uri: %s\n", uri);
		printf("s1 host: %s\n", get_header_value("Host", headers, num_headers));
		fflush(stdout);
	} else {
		printf("s1 is incomplete\n");
	}

	if (is_complete_request(s2)) {
		num_headers = parse_request(s2, method, hostname, port, uri, headers);
		printf("s2 method: %s\n", method);
		printf("s2 hostname: %s\n", hostname);
		printf("s2 port: %s\n", port);
		printf("s2 host: %s\n", get_header_value("Host", headers, num_headers));
	} else {
		printf("s2 is incomplete\n");
	}

	if (is_complete_request(s3)) {
		num_headers = parse_request(s3, method, hostname, port, uri, headers);
		printf("s3 method: %s\n", method);
		printf("s3 hostname: %s\n", hostname);
		printf("s3 port: %s\n", port);
		printf("s3 host: %s\n", get_header_value("Host", headers, num_headers));
	} else {
		printf("s3 is incomplete\n");
	}

}

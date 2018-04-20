/* NAME: Andrew Ding, Caleb Chau
 * EMAIL: andrewxding@gmail.com, caleb.h.chau@gmail.com
 * ID: 504748356, 204805602
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

using namespace std;

/* Error message constants */
const char* ACCEPT = "accept";
const char* FORK = "fork";
const char* READ = "read";
const char* WRITE = "write";
const char* SIGACTION = "sigaction";
/* Arbirtrary buffer size */
const size_t BUFFER_SIZE = 1024;
/* Check function return values */
int ret;
/* Various header field messages */
const string header_base = "HTTP/1.1 200 OK\r\nConnection: close\r\nServer: server/0.0.1\r\n";
const string fail_header = "HTTP/1.1 404 Not Found\r\nConnection: close\r\nServer: server/0.0.1\r\nContent-Type: text/html\r\n";
const string fail_html = "<h1>404 Page Not Found</h1>";
const string default_file_type = "application/octet-stream";
const string date_format = "%a, %d %b %Y %T GMT";
/* Map of file extensions to file types */
const unordered_map<string, string> file_types =
{
    { "htm", "text/html" },
    { "html", "text/html" },
    { "gif", "image/gif" },
    { "jpg", "image/jpeg" },
    { "jpeg", "image/jpeg" },
};

void sigchld_handler(int s) {
    (void) s;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void handle_error(const char* operation) {
    int err_no = errno;
    char* err_message = strerror(err_no);
    fprintf(stderr, "%s failed: %s\n", operation, err_message);
}

/* Read wrapper function */
ssize_t Read(int fd, void* buf, size_t count) {
    if ((ret = read(fd, buf, count)) == -1) {
        handle_error(READ);
        exit(1);
    } else {
        return ret;
    }
}

/* Write wrapper function */
ssize_t Write(int fd, const void* buf, size_t count) {
    if ((ret = write(fd, buf, count)) == -1) {
        handle_error(WRITE);
        exit(1);
    } else {
        return ret;
    }
}

/* Find wrapper function */
size_t Find(string search, size_t pos, string str){
    size_t index = str.find(search, pos);
    if (index == string::npos) {
        exit(0);
    }
    return index;
}

/* Main function to handle requests, used by each child process to form response message */
void handle_request(int client_sock_fd)
{
    int count;
    char buffer[BUFFER_SIZE];
    size_t start, end;

    string request, request_method, request_uri, request_version;
    string response_header, response_body, file_path;
    string file_type, extension;

    struct tm* tm;
    struct stat attr;

    /* Read from socket to get HTTP request header */
    count = Read(client_sock_fd, buffer, BUFFER_SIZE);

    /* Print HTTP request to console for Part A */
    request.append(buffer, count);
    printf("%s", request.c_str());

    start = end = 0;

    /* Get request_method */
    end = Find(" ", start, request);
    request_method = request.substr(start, end - start);
    start = end + 1;

    /* Get request_uri */
    end = Find(" ", start, request);
    request_uri = request.substr(start, end - start);
    start = end + 1;

    string temp;

    /* Handle file names with spaces by replacing any '%20' in request_uri with spaces */
    for (size_t i = 0; i < request_uri.length(); i++) {
        if (i < request_uri.length() - 2 && request_uri[i] == '%' && request_uri[i + 1] == '2' && request_uri[i + 2] == '0') {
            temp += ' ';
            i += 2;
        } else {
            temp += request_uri[i];
        }
    }

    request_uri = temp;

    /* Get first character after the last '.' in the request_uri */
    size_t pos = request_uri.rfind('.') + 1;

    /* Determine requested file type by looking at the extension in the request_uri */
    if (pos != string::npos) {
        extension = request_uri.substr(pos);

        /* Change extension to all lower case to make sure we find a match in our map */
        transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        /* Check map to find desired file type */
        unordered_map<string, string>::const_iterator encoding = file_types.find(extension);

        if (encoding != file_types.end()) {
            file_type = encoding->second;
        } else {
            file_type = default_file_type;
        }
    } else {
        file_type = default_file_type;
    }

    file_path = "." + request_uri;
    ret = stat(file_path.c_str(), &attr);
    ifstream ifs(file_path);

    /* File couldn't be opened or is a directory, use 404 Header and Body
     * Otherwise, put file type in header under Content-Type: and get Last-Modified: time for caching
     */
    if (ifs.fail() || (ret == 0 && S_ISDIR(attr.st_mode))) {
        response_header = fail_header;
        response_body = fail_html;
    } else {
        response_header = header_base + "Content-Type: " + file_type + "\r\n";

        /* Get file data to put in response_body */
        stringstream response_data;
        response_data << ifs.rdbuf();

        response_body = response_data.str();

        /* Create Last-Modified header */
        if (ret == 0) {
            tm = gmtime(&attr.st_mtime);

            if (strftime(buffer, BUFFER_SIZE, date_format.c_str(), tm) != 0) {
                response_header += "Last-Modified: " + string(buffer) + "\r\n";
            }
        }
    }

    /* Create Date header */
    time_t curtime;
    time(&curtime);
    tm = gmtime(&curtime);

    if (strftime(buffer, BUFFER_SIZE, date_format.c_str(), tm) != 0) {
        response_header += "Date: " + string(buffer) + "\r\n";
    }

    /* Add file data to response */
    response_header += "Content-Length: " + to_string(response_body.length()) + "\r\n\r\n" + response_body;

    /* Send HTTP response to browser */
    count = Write(client_sock_fd, response_header.c_str(), response_header.length());
}

int main(int argc, char* argv[]) {
    int sock_fd, client_sock_fd;
    socklen_t client_size;
    pid_t child_pid;
    char* port_number;

    struct addrinfo flags, * server_info, * p;
    struct sockaddr_in client_address;

    struct sigaction sa;

    /* Port option is mandatory */
    if (argc < 2) {
        fprintf(stderr, "./server: incorrect usage\nMust provide a port number\n");
        exit(1);
    }

    port_number = argv[1];

    memset(&flags, 0, sizeof(flags));

    flags.ai_family = AF_UNSPEC;
    flags.ai_socktype = SOCK_STREAM;
    flags.ai_flags = AI_PASSIVE;

    /* Find addresses that match provided flags and port_number */
    if ((ret = getaddrinfo(NULL, port_number, &flags, &server_info)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        exit(1);
    }

    /* Loop through results and bind to first address possible */
    for (p = server_info; p != NULL; p = p->ai_next) {
        if ((sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }

        if (bind(sock_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock_fd);
            continue;
        }

        /* Successful bind to address */
        break;
    }

    /* Looped through entire list w/o a successful bind */
    if (p == NULL) {
        fprintf(stderr, "bind failed: Could not bind to socket\n");
        exit(1);
    }

    /* Done, make sure no memory leaks */
    freeaddrinfo(server_info);

    listen(sock_fd, 5);
    client_size = sizeof(client_address);

    /* Kill zombie processes */
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
        handle_error(SIGACTION);
        exit(1);
    }

    /* Listen to incoming connections and hand off connections to be handled by child process */
    while (1) {
        if ((client_sock_fd = accept(sock_fd, (struct sockaddr*) &client_address, &client_size)) == -1) {
            handle_error(ACCEPT);
            exit(1);
        }

        child_pid = fork();

        if (child_pid > 0) { /* Parent process */
            close(client_sock_fd);
        } else if (child_pid == 0) { /* Child process */
            close(sock_fd);
            handle_request(client_sock_fd);
            close(client_sock_fd);
            exit(0);
        } else { /* fork() failed */
            handle_error(FORK);
            exit(1);
        }
    }

    /* Everything works!! */
    exit(0);
}

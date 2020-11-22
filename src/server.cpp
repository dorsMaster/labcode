#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <chrono>
#include <iostream>
#include <fcntl.h>
#include <sys/time.h>
#include <iomanip>
#include <unordered_map>
#include "../headers/tands.h"

using namespace std;

time_t      endwait, start;
int         i               =   1;
time_t      seconds         =  30;
char        separator       = ' ';
const int   timeWidth       =  12;
const int   statusWidth     =   6;
const int   numWidth        =   3;
const int   singleWidth     =   1;

template<typename T> void printElement(T t, const int& width) {
    cout << left << setw(width) << setfill(separator) << t;
}

template<typename T> void printNumElement(T t, const int& width) {
    cout << right << setw(width) << setfill(separator) << t;
}

template<typename T> void printTimeElement(T t, const int& width) {

    cout << fixed <<setprecision(3) << left << setw(width) << setfill(separator)
         << t
         << ": #";
}

void printRow(int i, char job, string id){
    const auto startTask = std::chrono::system_clock::now();
    printTimeElement(std::chrono::duration_cast<std::chrono::milliseconds>(startTask.time_since_epoch()).count()/1000, timeWidth);
    printElement(i, numWidth);
    printElement("(", singleWidth);
    printElement(job, singleWidth);
    printNumElement(id, numWidth);
    printElement(")", singleWidth);
    printElement(" from", statusWidth);
    cout << "\n";
}

int guard(int n, char * err) { if (n == -1) { perror(err); exit(1); } return n; }

void taskForClient(int count, int client_fd, char buffer[]){
    start = time(NULL);
    count = read(client_fd, buffer, sizeof(buffer));
    if(count > 0) {
        buffer[count] = '\0';
        if (buffer[0]=='T') {
            string tmp = string(buffer).substr(1, string(buffer).length() - 1);
            printRow(i, buffer[0], tmp.c_str());
            Trans(atoi(tmp.c_str()));
            const auto endTask = std::chrono::system_clock::now();
            printRow(i, 'D', "one");
            i++;
        }
        endwait = start + seconds;
    }
}

void tryForConnection(int fd){
    start = time(NULL);
    struct sockaddr_in client_address; // client address
    int len = sizeof(client_address);
    int client_fd = accept(fd, (struct sockaddr*) &client_address, reinterpret_cast<socklen_t *>(&len));  // accept connection

//    if (client_fd < 0) // bad connection

    char buffer[1024];
    int count = 0;

    do {
        taskForClient(count, client_fd, buffer);
        string client_response = "D" + to_string(i);
        write(client_fd, client_response.c_str(), (ssize_t)strlen(client_response.c_str()));
    } while(count > 0);

    close(client_fd);
}


int main(int argc, char *argv[]) {
    int portNum = strtol(argv[1], NULL, 10);

    int fd = socket(AF_INET, // an Internet socket
                    SOCK_STREAM, // reliable stream-like socket
                    0); // OS determine the protocol (TCP)

    if (fd < 0)
        return 1; // something went wrong

    int flags = guard(fcntl(fd, F_GETFL), "could not get flags on TCP listening socket");
    guard(fcntl(fd, F_SETFL, flags | O_NONBLOCK), "could not set TCP listening socket to be non-blocking");


    struct sockaddr_in serv_addr;

    // setup server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portNum); // port

    if (bind(fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) // bind socket to the server address
        return 1;

    printf("Listening on port %d for clients.\n", portNum);
    if (listen(fd, 1) < 0) // wait for clients, only 1 is allowed.
        return 1;

    start = time(NULL);
    endwait = start + seconds;
    while(start < endwait) {
        tryForConnection(fd);
    }

    close(fd);
    return 0;
} 
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
#include <thread>
#include "../headers/tands.h"

using namespace std;

int guard(int n, char * err) { if (n == -1) { perror(err); exit(1); } return n; }

void setInterval(auto function,int interval) {
    thread th([&]() {
        while(true) {
            Sleep(interval);
            function();
        }
    });
    th.detach();
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

    int i = 0;
    bool time_out = false;
    time_t endwait;
    time_t start = time(NULL);
    cout << start << endl;
    time_t seconds = 30;
    endwait = start + seconds;
    while(1 && (start < endwait)) {
        start = time(NULL);
        struct sockaddr_in client_address; // client address
        int len = sizeof(client_address);
        int client_fd = accept(fd, (struct sockaddr*) &client_address, reinterpret_cast<socklen_t *>(&len));  // accept connection

        if (client_fd < 0) // bad connection
            continue; // discard

        char buffer[1024];
        int count = 0;

        do {
            start = time(NULL);
            count = read(client_fd, buffer, sizeof(buffer));
            if(count > 0) {
                buffer[count] = '\0';
                if (buffer[0]=='T') {
                    const auto startTask = std::chrono::system_clock::now();
                    cout << std::chrono::duration_cast<std::chrono::milliseconds>(startTask.time_since_epoch()).count()/1000
                         << " : #" << i << "   (" << buffer << "  )   from" << endl;
                    string tmp = string(buffer).substr(1, string(buffer).length() - 1);
                    Trans(atoi(tmp.c_str()));
                    const auto endTask = std::chrono::system_clock::now();
                    cout << std::chrono::duration_cast<std::chrono::milliseconds>(endTask.time_since_epoch()).count()/1000
                         << " : #" << i << "   (Done)   from" << endl;
                    i++;
                }
            }
            endwait = start + seconds;
        } while(count > 0);

        close(client_fd);

    }
    cout<<endwait<<endl;
    close(fd);
    return 0;
} 
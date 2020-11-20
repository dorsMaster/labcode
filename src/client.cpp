#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <chrono>
#include "../headers/tands.h"

using namespace std;

int main(int argc, char *argv[]) {

    char hostname[1024];
    gethostname(hostname, 1024);
    printf("Host %s \n", hostname);

    int numTransactions = 0;

    int portNum = strtol(argv[1], NULL, 10);
    char *ipAdd = argv[2];
    if (portNum > 64000 || portNum < 5000) {
        printf("Please enter a valid port number.");
        return 0;
    }

    char *msg;

    string val;
    while (getline(cin, val)) {
        msg = const_cast<char *>(val.c_str());

        if (val[0] == 'S') {
            string tmp = val.substr(1, val.length() - 1);
            Sleep(atoi(tmp.c_str()));
        }
        else
            {        //Send some data
                int fd = socket(AF_INET, // an Internet socket
                                SOCK_STREAM, // reliable stream-like socket
                                0); // OS determine the protocol (TCP)

                if (fd < 0)
                    return 1; // something went wrong

                struct sockaddr_in serv_addr;

                // setup server address
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_addr.s_addr = inet_addr(ipAdd);
                serv_addr.sin_port = htons(portNum); // port
            if (connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) // bind socket to the server address
                return 1;

                const auto startTask = std::chrono::system_clock::now();
                cout << std::chrono::duration_cast<std::chrono::milliseconds>(startTask.time_since_epoch()).count()/1000
                     << " :   Send" << "   (" << msg << "  )   from" << endl;
            if (send(fd, msg, strlen(msg), 0) < 0) {
                puts("Send failed");
                return 1;
            }
            write(fd, msg, sizeof(msg));

            int sent = 0;
            while ((sizeof(msg) - sent) > 0)
                sent += write(fd, msg + sent, sizeof(msg) - sent);

            numTransactions++;
            close(fd); // close connection
        }
    }
    cout << "Sent " << numTransactions << " transactions" << endl;
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include "../headers/tands.h"

//TODO make more methods!!

using namespace std;

char        separator       = ' ';
const int   timeWidth       =  12;
const int   statusWidth     =   6;
const int   numWidth        =   4;
const int   singleWidth     =   1;

template<typename T> void printElement(T t, const int& width) {
    cout << left << setw(width) << setfill(separator) << t;
}

template<typename T> void prinNumElement(T t, const int& width) {
    cout << right << setw(width) << setfill(separator) << t;
}

template<typename T> void printTimeElement(T t, const int& width) {
    cout << fixed <<setprecision(2) << left << setw(width) << setfill(separator)
    << t
    << ": ";
}

void printRow(string status, char job, int id){
    const auto startTask = std::chrono::system_clock::now();
    double tm = std::chrono::duration_cast<std::chrono::milliseconds>(startTask.time_since_epoch()).count()/1000.0;
    printTimeElement(tm, timeWidth);    printElement(status, statusWidth);
    printElement("(", singleWidth);
    printElement(job, singleWidth);
    prinNumElement(id, numWidth);
    printElement(")", singleWidth);
    cout << "\n";
}

int main(int argc, char *argv[]) {
    int numTransactions = 0;

    int portNum = strtol(argv[1], NULL, 10);
    char *ipAdd = argv[2];
    if (portNum > 64000 || portNum < 5000) {
        printf("Please enter a valid port number.");
        return 0;
    }

    cout << "Using port " << portNum << endl;

    cout << "Using server address " << ipAdd << endl;

    char hostname[1024];
    string hostname_pid;
    gethostname(hostname, 1024);
    hostname_pid = (string)hostname + "." + to_string(getpid());
    cout << "Host " << hostname_pid << endl;


    string val;
    while (getline(cin, val)) {
        string msgtopass = hostname_pid + "," + val;
        const char *msg = msgtopass.c_str();

        if (val[0] == 'S') {
            string tmp = val.substr(1, val.length() - 1);
            Sleep(atoi(tmp.c_str()));
            cout << "Sleep " << tmp << " units" << endl;
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

            if (send(fd, msg, strlen(msg), 0) < 0) {
                puts("Send failed");
                return 1;
            }

            string val_int = val.substr(1, string(msg).length() - 1);
            printRow("Send", val[0], atoi(val_int.c_str()));

            char server_reply[1000];
            memset(server_reply, 0, 1000);
            if (recv(fd, server_reply, 1000, 0) < 0) {
                puts("recv failed");
                break;
            }
            string serverreplytmp = string(server_reply).substr(1, string(server_reply).length() - 1);
            printRow("Recv", server_reply[0], atoi(serverreplytmp.c_str()));
            numTransactions++;
            close(fd); // close connection
        }
    }
    cout << "Sent " << numTransactions << " transactions" << endl;
    return 0;
}


#include "../headers/includes.h"

/**
 * Uses a template to assign width to table for when it prints
 * @param t
 * @param width
 */
template<typename T> void printElement(T t, const int& width) {
    cout << left << setw(width) << setfill(separator) << t;
}

/**
 * Uses a template to assign width to table for when it prints specific to numbers
 * @param t
 * @param width
 */
template<typename T> void prinNumElement(T t, const int& width) {
    cout << right << setw(width) << setfill(separator) << t;
}

/**
 * Uses a template to assign width to table for when it prints specific to time element.
 * it also set the precision of the number to be 2 decimal places
 * @param t
 * @param width
 */
template<typename T> void printTimeElement(T t, const int& width) {
    cout << fixed <<setprecision(2) << left << setw(width) << setfill(separator)
         << t
         << ": ";
}

/**
 * Uses a template to assign width to table for when it prints and then prints one row
 * @param status
 * @param job
 * @param id
 */
void printRow(string status, char job, int id){
    const auto startTask = std::chrono::system_clock::now();
    double tm = std::chrono::duration_cast<std::chrono::milliseconds>(startTask.time_since_epoch()).count()/1000.0;
    printTimeElement(tm, timeWidth);    printElement(status, statusWidth);
    printElement("(", singleWidth);
    printElement(job, singleWidth);
    prinNumElement(id, indexWidth);
    printElement(")", singleWidth);
    cout << "\n";
}

/**
 * Gets the hostname
 * @return string
 * */
string getHostName(){
    char hostname[1024];
    string hostname_pid;
    gethostname(hostname, 1024);
    hostname_pid = (string)hostname + "." + to_string(getpid());
    cout << "Host " << hostname_pid << endl;
    return hostname_pid;
}

/**
 * reads the port number and ip address of the client, it establishes a connection
 * reads from a file, sends a task to the server, it sleeps if given the sleep command
 * it receives from the server whether the task is done, if done, it closes the connection
 * @param argv
 * @param argc
 */
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

    string hostname_pid = getHostName();

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


#include "../headers/includes.h"

time_t      endwait, start;
double      startOfProgram, endOfProgram;
bool        startOfProgramFlag  = true;
unordered_map<string,int>      summary;
/**
 * Uses a template to assign width to table for when it prints
 * @param t
 * @param width
 */
template<typename T> void printElement(T t, const int& width) {
    cout << left << setw(width) << setfill(separator) << t;
}

/**
 * Uses a template to assign width to table for when it prints specific for the summary table
 * @param t
 * @param width
 */
template<typename T> void printSummaryElement(T t, const int& width) {
    cout << fixed << setprecision(1) << left << setw(width) << setfill(separator) << t << "transactions/sec  " ;
}

/**
 * Uses a template to assign width to table for when it prints specific to numbers
 * @param t
 * @param width
 */
template<typename T> void printNumElement(T t, const int& width) {
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
         << t;
}

/**
 * Updates a hash map of the servers and the number of transactions that it has done for them
 * @param hostname
 * */
void updateSummary(string hostname){
    if (summary.find(hostname) == summary.end())
        summary[hostname]=1;
    else
        summary[hostname]++;
}

/**
 * Uses a template to assign width to table for when it prints and then prints one row
 * @param i
 * @param job
 * @param id
 * @param hostname
 */
void printRow(int i, char job, string id, string hostname){
    const auto startTask = std::chrono::system_clock::now();
    double tm = std::chrono::duration_cast<std::chrono::milliseconds>(startTask.time_since_epoch()).count()/1000.0;
    if (startOfProgramFlag){
        startOfProgram = tm;
        startOfProgramFlag = false;
    }
    endOfProgram = tm;
    printTimeElement(tm, timeWidth);
    cout << ": #  ";
    printElement(i, indexWidth);
    printElement("(", singleWidth);
    printElement(job, singleWidth);
    printNumElement(id, numWidth);
    printElement(")", singleWidth);
    printElement(" from ", statusWidth);
    cout << hostname << "\n";
}

/**
 * To handle error and print error statements
 * @param n
 * @param err
 * @return int
 */
int guard(int n, string err) { if (n == -1) { perror(err.c_str()); exit(1); } return n; }

/**
 * This is where the client gets assigned the task that is required to do
 * it calls trans and also prints done when the task has been executed
 * @param count
 * @param client_fd
 * @return int
 */
int taskForClient(int count, int client_fd){
    char buffer[1024];
    start = time(NULL);
    count = read(client_fd, buffer, sizeof(buffer));

    if(count > 0) {
        buffer[count] = '\0';
        string hostname;
        int indexOColon, j = 0;
        while(buffer[j]!=','){
            hostname+=buffer[j];
            j++;
        }
        indexOColon = j;

        if (buffer[indexOColon+1]=='T') {
            string tmp = string(buffer).substr(indexOColon+2, string(buffer).length() - 1);
            printRow(i, buffer[indexOColon+1], tmp.c_str(), hostname);
            Trans(atoi(tmp.c_str()));
            const auto endTask = std::chrono::system_clock::now();
            printRow(i, 'D', "one", hostname);
            i++;
            updateSummary(hostname);
        }
        endwait = start + seconds;
    }
    return count;
}

/**
 * Tries to establish a connection with the client
 * @param fd
 * */
void tryForConnection(int fd){
    start = time(NULL);
    struct sockaddr_in client_address; // client address
    int len = sizeof(client_address);
    int client_fd = accept(fd, (struct sockaddr*) &client_address, reinterpret_cast<socklen_t *>(&len));  // accept connection

    int count = 0;

    do {
        count = taskForClient(count, client_fd);
        string client_response = "D" + to_string(i);
        ssize_t written = write(client_fd, client_response.c_str(), (ssize_t)strlen(client_response.c_str()));
        if (written < 0 && count > 0)
        {
            perror("write() failed");
        }
    } while(count > 0);

    close(client_fd);
}

/**
 * Prints the summary of all the transactions from different clients by parsing through the hash
 */
void printSummary(){
    cout << "SUMMARY" << endl;
    int totTrans = 0;
    for (auto& host: summary) {
        printNumElement(host.second,indexWidth);
        cout << " transactions from " << host.first << endl;
        totTrans+=host.second;
    }
    printSummaryElement(totTrans/(endOfProgram-startOfProgram),tpsWidth);
    cout << "(" << totTrans << "/";
    printTimeElement(endOfProgram-startOfProgram,singleWidth);
    cout << ")" << endl;
}

/**
 * Reads the port number, creates a socket, listens, and times out after 30 seconds of no connection received
 * prints the summary
 * @param argc
 * @param argv
 */
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

    printf("Using port %d \n", portNum);
    if (listen(fd, SOMAXCONN) < 0) // wait for clients, only 1 is allowed.
        return 1;

    start = time(NULL);
    endwait = start + seconds;
    while(start < endwait) {
        tryForConnection(fd);
    }
    printSummary();

    close(fd);
    return 0;
} 
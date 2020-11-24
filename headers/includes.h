#ifndef LABCODE_INCLUDES_H
#define LABCODE_INCLUDES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <chrono>
#include <iostream>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <iomanip>
#include <unordered_map>
#include "../headers/tands.h"
using namespace std;

int         i                   =   1;
time_t      seconds             =  30;
char        separator           = ' ';
const int   timeWidth           =  12;
const int   statusWidth         =   6;
const int   tpsWidth            =   5;
const int   indexWidth          =   4;
const int   numWidth            =   3;
const int   singleWidth         =   1;
#endif //LABCODE_INCLUDES_H

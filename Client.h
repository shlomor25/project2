/*****************************
 * Project of:
 * Ori Kopel 205533151
 * Shlomo Rabinovich 308432517
 * December 18
 ******************************/

#ifndef PROJECT_CLIENT_H
#define PROJECT_CLIENT_H

#include <cstdio>
#include <cstdlib>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string>
#include <mutex>
#include "Global.h"

string msgToServer;
mutex clientLocker;


/**
 * Connect to client.
 * @param portNumber
 * @param ipPath
 */
void ConnectClient(const int portNumber, const string ipPath) {
    cout << "client started" << endl;
    int n, sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    server = gethostbyname(ipPath.c_str());

    if (server == nullptr) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portNumber);

    /* Now connect to the server */
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "ERROR connecting" << endl;
    }
    /* Send message to the server */
    while (true) {
        if (msgToServer.empty()) {
            usleep(10);
            continue;
        }
        clientLocker.lock();
        const char *c = msgToServer.c_str();
        clientLocker.unlock();
        n = send(sockfd, c, strlen(c), MSG_EOR);
        clientLocker.lock();
        msgToServer = "";
        clientLocker.unlock();

        if (n < 0) {
            cout << "ERROR writing to socket" << endl;
            exit(1);
        }
        /* Now read server response */
        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);
        if (n < 0) {
            cout << "ERROR reading from socket" << endl;
        }
    }
}


/**
 * send message to simulator
 * @param address - path of var to set
 * @param val - new value
 */
void sendToClient(const string address, double val) {
    try {
        clientLocker.lock();
        msgToServer += "set " + address + " " + to_string(val) + "\r\n";
        clientLocker.unlock();
    } catch (...) { throw "connection failed"; }

}

#endif //PROJECT_CLIENT_H

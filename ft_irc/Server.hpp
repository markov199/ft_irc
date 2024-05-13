
#pragma once

#include "Client.hpp"
#include "Channel.hpp"
#include "Command.hpp"
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstdio>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <utility>
#include <map>
#include <csignal>

#define MAX_CON 100
#define MAX_BUF 512
#define HOSTNAME ("mumsServer42.com")
#define RESET "\033[30m"
#define BLUE "\033[34m"

class Client;
class Channel;

class Server
{
	public:
		Server(int &port, std::string &password);
		~Server();
	
		void run(); // do all to start server
		void createSocket();
		void waitConnections();
		void acceptConnection();
		void readMessage(int indexFd);
		void sendResponse(int clientSocket, std::string msg);
		void removeClient(int clientSocket);
		void cleanUp();

		//getters
		std::map<int, Client *>				getClientsArray() const;
		Client *							getClient(int clientSocket) const;
		Client *							getClient(std::string name);
		std::map<std::string, Channel *>	getChannelsArray() const;
		Channel *							getChannel(std::string channelName);
		bool								channelExists(std::string name);
		int									compare(std::string s1, std::string s2) const;
		std::string							getPassword() const;
		std::string							getHostName() const;
		void								removeChannel(std::string name);
		std::map<std::string, Channel *>	_channelsArray;

	private:
		int									_port;
		std::string							_password;
		std::string							_ipAddress;
		std::string							_hostName;
		int									_listenSocket;
		std::vector<pollfd>					_pfds;
		std::map<int, Client *>				_clientsArray;
		
		// constructors in private because dont want to instantiate with no param nor copy
		Server();
		Server(Server const &);
		Server &operator=(Server const &);
};


#include "Server.hpp"

bool running = true;
void signalHandler(int sigNum)
{
	(void)sigNum;
	running = false;
}

Server::Server() {}
Server::Server(Server const &) { }
Server &Server::operator=(Server const &) { return *this; }

Server::Server(int &port, std::string &password)
	: _channelsArray(),
	  _port(port), 
	  _password(password), 
	  _ipAddress("127.0.0.1"),
	  _hostName(HOSTNAME),
	  _listenSocket(-1),
	  _pfds(),
	  _clientsArray()
{
	std::cout << "---------------------------------------------" <<std::endl;
	std::cout << "|                                           |" <<std::endl;
	std::cout << "|  ####  ###### #####  #    # ###### #####  |" <<std::endl;
	std::cout << "| #      #      #    # #    # #      #    # |" <<std::endl;
	std::cout << "|  ####  #####  #    # #    # #####  #    # |" <<std::endl;
	std::cout << "|      # #      #####  #    # #      #####  |" <<std::endl;
	std::cout << "| #    # #      #   #   #  #  #      #   #  |" <<std::endl;
	std::cout << "|  ####  ###### #    #   ##   ###### #    # |" <<std::endl;
	std::cout << "|                                           |" <<std::endl;
	std::cout << "---------------------------------------------" <<std::endl;
}

Server::~Server() {
	cleanUp();
}

void Server::run() 
{
	createSocket();
	waitConnections();
}

void Server::createSocket() {
	
	int fd = socket(AF_INET, SOCK_STREAM, 0); // oppening fd for a socket
    if (fd < 0) {
		std::cerr << "Fail to create a socket" << std::endl;
		exit (1);
	}

	// to clear the socket that was connected is still hanging around in the kernel, use setsockopt
    int optval = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
		close (fd);
		std::cerr << "Fail to clear the socket" <<std::endl;
		exit (1);
	}

	// making socket to non block
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		close (fd);
		std::cerr << "Fail to change the Non-Block flag of the listener socket" << std::endl;
		exit (1);
	}

	// bind the socket to an IP and Port
	sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(_ipAddress.c_str());
	serverAddr.sin_port = htons(_port);
	if (bind(fd, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		close(fd);
		std::cerr <<"Fail to bind IP/port" <<std::endl;
		exit (1);
	}

	if (listen(fd, MAX_CON) < 0) // change the status of the socket to listenSocket
	{
		close(fd);
		std::cerr <<"Fail to make the socket fd to listen status" <<std::endl;
		exit (1);
	}
	_listenSocket = fd;
}

void Server::waitConnections() {
	
	// init pollfd struct and add server socket to the list
	pollfd serverPollfd;
	std::string msg;
    serverPollfd.fd = _listenSocket;
    serverPollfd.events = POLLIN;
    serverPollfd.revents = 0;
    _pfds.push_back(serverPollfd);

	std::cout << ">>> Waiting for clients..." <<std::endl;
	signal(SIGINT, signalHandler);
	signal(SIGTSTP, signalHandler);
	
	while (true)
	{
		if (!running)
			break ;
		if (poll(_pfds.data(), _pfds.size(), 5) < 0) 
		{
			cleanUp();
			break ;
		}
		for (unsigned int i = 0; i < _pfds.size(); i++)
        {
            if (_pfds[i].revents & POLLIN) // checking all sockets
            {
				if (_pfds[i].fd == _listenSocket) // check if some info has been received from socket
                    acceptConnection();
                else 							// now coming from a accepted client, so read the command
                    readMessage(i);
            }
			if(_pfds[i].revents & POLLOUT)
			{
				Client *client = this->getClient(_pfds[i].fd);
				if (client != NULL && !client->toBeSent.empty())
				{
					msg = client->toBeSent.front();
					send(_pfds[i].fd, msg.c_str(), msg.size(), 0);
					client->toBeSent.pop_front();
				}
			}
        }
	}
}

void Server::acceptConnection() {
	
	// create info for new client and new socket
	sockaddr_in clientAddr;
	memset(&clientAddr, 0, sizeof(clientAddr));
	socklen_t	clientSize = sizeof(clientAddr);

	int clientSocket = accept(_listenSocket, (sockaddr *)&clientAddr, &clientSize);
	if (clientSocket < 0) {
		std::cerr <<"Can't connect with new client" <<std::endl;
		return ; // maybe here instead of exiting, just return..?
	}

	// if new socket open, include client to poll
	pollfd clientPollfd;
    clientPollfd.fd = clientSocket;
    clientPollfd.events = POLLIN | POLLOUT;
    clientPollfd.revents = 0;
    _pfds.push_back(clientPollfd);
	
    // now include new client to ClientArray
	std::string ip = inet_ntoa(clientAddr.sin_addr);
	Client *client = new Client(clientSocket, ip);
	_clientsArray.insert(std::make_pair(clientSocket, client));

	// display client get connected
	std::cout <<">> New Client Connected on socket fd: " <<clientSocket <<std::endl;
	sendResponse(clientSocket, BLUE ">> To use the server you need to send the commands: PASS, NICK and USER to register yourself!\r\n" RESET);
}

void Server::readMessage(int i) {
	int clientFd = _pfds[i].fd;
	char buffer[MAX_BUF];
	memset(buffer, 0, sizeof(buffer));
	int bytesRecv = recv(clientFd, buffer, sizeof(buffer), 0);
	if (bytesRecv <= 0) {
		if (bytesRecv == 0) // only when close terminal
			std::cerr <<">> Client disconnected on socket fd: " <<clientFd <<std::endl;
		else // problem on socketfd
			std::cerr <<">> Connection problem" <<std::endl;
		
		// now deleting client that disconected
		removeClient(clientFd);
		return ;
	}
	// when there is something coming from poll
	_clientsArray.find(clientFd)->second->setRequestMessage(buffer);
	if (!_clientsArray.find(clientFd)->second->getRequestMessage().empty())
	{
		std::string msg = _clientsArray.find(clientFd)->second->getRequestMessage();
		Command *cmd = new Command;
		std::cout << msg << "\n";
		cmd->execute(clientFd, msg, this);
		delete cmd;
	}
}

void Server::removeClient(int clientSocket) {
	
	std::vector<std::string> channels = getClient(clientSocket)->getChannelList();
	Client *user = getClient(clientSocket);
	Channel *chn;
	std::string prefix = user->doPrefix();
	for (int i = 0; i < (int)channels.size(); i++)
	{
		chn = getChannel(channels.at(i));
		std::map<std::string, int> usersMap = chn->getUsersArray();
		std::map<std::string, int>::iterator itUsersBegin = usersMap.begin();
		std::map<std::string, int>::iterator itUsersEnd = usersMap.end();
		while (itUsersBegin != itUsersEnd) { // broadcast to all
			sendResponse(itUsersBegin->second, RPL_PART(prefix, chn->getName(), ""));
			itUsersBegin++;
		}
		chn->leaveChannel(user->getNickName());
		user->unsetChannelList(chn->getName());
		if (chn->getNumOfClients() == 0)
			removeChannel(chn->getName());
	}
	// deleting client
	std::map<int, Client *>::iterator it = _clientsArray.find(clientSocket);
	delete (it->second);
	_clientsArray.erase(it);
	close(clientSocket);

	//removing from poll
	std::vector<pollfd>::iterator it2;
	for (it2 = _pfds.begin(); it2 != _pfds.end(); it2++) {
		if (it2->fd == clientSocket){
			_pfds.erase(it2);
			break ;
		}
	}
}

void Server::sendResponse(int clientSocket, std::string msg) {
	Client *client = this->getClient(clientSocket);
	if (client != NULL)
		client->toBeSent.push_back(msg);
}

void Server::cleanUp() {
	// close the fds oppened
	if (!_pfds.empty()) {
		for (unsigned int i = 0; i < _pfds.size(); i++) {
			if (_pfds[i].fd > 0) {
				close(_pfds[i].fd);
				_pfds.erase(_pfds.begin() + i);
			}
		}
		_pfds.clear();
	}
	// delete Clients
	if (!_clientsArray.empty()) {
        std::map<int, Client *>::iterator it; 
        for(it = _clientsArray.begin(); it != _clientsArray.end(); it++) {
            delete(it->second);
		}
		_clientsArray.clear();
    }
	// delete Channels
    if (!_channelsArray.empty()) {
        std::map<std::string, Channel *>::iterator it2; 
        for(it2 = _channelsArray.begin(); it2 != _channelsArray.end(); it2++)
            delete(it2->second);
		_channelsArray.clear();
    }
}

std::map<int, Client *> Server::getClientsArray() const {
	return (_clientsArray);
}

std::string Server::getPassword() const {
	return (_password);
}

Client *	Server::getClient(int clientSocket) const {
	if (_clientsArray.find(clientSocket) != _clientsArray.end())
		return(_clientsArray.find(clientSocket)->second);
	return (NULL);
}

std::string	Server::getHostName() const{
	return (this->_hostName);
}

std::map<std::string, Channel *> Server::getChannelsArray() const {
	return (_channelsArray);
}

Channel *	Server::getChannel(std::string channelName)
{ // what happens if no channel is there;
	std::map<std::string, Channel *>::iterator chItr;
	
	for (chItr = this->_channelsArray.begin(); chItr != _channelsArray.end(); chItr++)
	{
		if ((compare(chItr->second->getName(), channelName) == 0))
			return (chItr->second); //channel exists
	}
	return (NULL);
}

bool Server::channelExists(std::string name)
{
	std::map<std::string, Channel *>::iterator chItr;
	
	for (chItr = _channelsArray.begin(); chItr != _channelsArray.end(); chItr++)
	{
		if ((compare(chItr->second->getName(), name) == 0))
			return (1); //channel exists
	}
	return (0);

}

int	Server::compare(std::string s1, std::string s2) const
{
	if (s1.length() != s2.length())
		return (1);
	for (int i = 0; i < (int)s1.length(); i++)
		if ((char)toupper(s1[i]) != (char)toupper(s2[i]))
			return (1);
	return (0);		
}

Client	*Server::getClient(std::string name)
{
	std::map<int, Client *>::iterator itr;

	for(itr = this->_clientsArray.begin(); itr != this->_clientsArray.end(); itr++)
	{
		if (itr->second->getNickName().c_str() == name)
			return (itr->second);
	}
	return (NULL);
}

void	Server::removeChannel(std::string name)
{
	std::map<std::string, Channel *>::iterator	it;

	it = this->_channelsArray.find(name);
	if (it != this->_channelsArray.end())
	{
		delete (it->second);
		this->_channelsArray.erase(it);
	}
}


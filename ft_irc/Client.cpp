
#include "Client.hpp"

// these first constructors only included cause canonical form mandatory
Client::Client() {}
Client::Client(Client const &src) { *this = src; }
Client &Client::operator=(Client const &rhs) {
	if (this == &rhs)
		return *this;
	return *this;
}

Client::~Client() {
}

Client::Client(int &fd, std::string &ip): _fd(fd), _port(6667), _IpAddressClient(ip),
_nickName(), _userName(), _fullName(),
_isAuthenticated(false), _isRegistered(false), _isOperator(false),
_passAuth(false), _nickAuth(false), _userAuth(false), _requestMessage(), _tempMessage() {}
 
// getters
int 		Client::getFd() const { return _fd; }
int			Client::getPort() const { return _port; }
std::string Client::getNickName() const { return _nickName; }
std::string Client::getUserName() const { return _userName; }
std::string Client::getFullName() const { return _fullName; }
std::string Client::getIpAddressClient() const { return _IpAddressClient; }
bool 		Client::getIsAuthenticated() const { return _isAuthenticated; }
int 		Client::getIsRegistered() const { return _isRegistered; }
int 		Client::getIsOperator() const { return _isOperator; }
std::string Client::getRequestMessage() const { return _requestMessage; }
bool		Client::getPassAuth() const { return (this->_passAuth); }
bool		Client::getNickAuth() const { return (this->_nickAuth); };

std::vector<std::string>    Client::getChannelList() {return (this->_myChannels);}

// setters
void	Client::setRequestMessage(char msg[])
{
	
	if (!_requestMessage.empty()) {
		_requestMessage.clear();
	}
	
	if (strstr(msg, "\n") == 0) {
		_tempMessage.append(msg);
	}

	else {
		_tempMessage.append(msg);
		_requestMessage.append(_tempMessage);
		if ((_requestMessage.length() >= 1) && (_requestMessage.at(_requestMessage.length() -1) == '\n'))
			_requestMessage.erase(_requestMessage.length() - 1, 1);
		if ((_requestMessage.length() >= 1) && (_requestMessage.at(_requestMessage.length() -1) == '\r'))
			_requestMessage.erase(_requestMessage.length() - 1, 1);
		_tempMessage.clear();
	}
	memset(msg, 0, MAX_BUF);
}

std::string Client::doPrefix()
{
	std::string prefix = _nickName + "!" + _userName + "@" + _IpAddressClient;
	return (prefix);
}

bool	Client::isInChannel(std::string channel)
{
	for (int i = 0; i < (int)_myChannels.size(); i ++)
	{
		if (_myChannels.at(i) == channel)
			return (1); // is in channel
	}
	return (0);
	
}

void	Client::authenticate()
{
	if (this->_passAuth == true && this->_userAuth == true && this->_nickAuth == true)
		this->setIsAuthenticated(1);
}

void	Client::setPassAuth(int i)
{
	this->_passAuth = i;
}

void	Client::setNickAuth(int i)
{
	this->_nickAuth = i;
}

void	Client::setUserAuth(int i)
{
	this->_userAuth = i;
}

void	Client::setIsAuthenticated(int i)
{
	this->_isAuthenticated = i;
}

void	Client::setNickName(std::string name)
{
	this->_nickName = name;
}

void	Client::setUserName(std::string name)
{
	this->_userName = name;
}

void	Client::setChannelList(std::string channelName)
{
	if (this->isInChannel(channelName) == 0)
		this->_myChannels.push_back(channelName);
}

void	Client::unsetChannelList(std::string channelName)
{
	for (int i = 0; i < (int)this->_myChannels.size(); i ++)
		if (this->_myChannels.at(i) == channelName)
			this->_myChannels.erase(this->_myChannels.begin() + i);
}

int	Client::compare(std::string s1, std::string s2) const
{
	if (s1.length() != s2.length())
		return (1);
	for (int i = 0; i < (int)s1.length(); i++)
		if ((char)toupper(s1[i]) != (char)toupper(s2[i]))
			return (1);
	return (0);		
}

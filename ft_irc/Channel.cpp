
#include "Channel.hpp"
#include "Command.hpp"

Channel::Channel() {}

Channel::Channel(Channel const &) {}

Channel	&Channel::operator=(Channel const &) { return (*this); }

Channel::~Channel() {}

Channel::Channel (std::string &name, int &clientFd, std::string &nickname) : _name(name)
{
	this->_nbclients = 1;
	this->_prefix = this->_name[0];
	this->_key = "";
	this->_topic = "";
	this->_users.insert (std::pair<std::string, int> (nickname, clientFd));
	this->_operators.insert (std::pair<std::string, int> (nickname, clientFd)); // check here later for prefix '+' to see if we add this operator or no
	this->_iMode = false;
	this->_tMode = false;
	this->_kMode = false;
	this->_maxNbClients = INT_MAX;
}

std::string	Channel::getName(void) const { return (this->_name); }

char		Channel::getPrefix(void) const { return (this->_prefix); }

int 		Channel::getNumOfClients(void) const { return (this->_nbclients);}

std::string Channel::getKey(void) const { return (this->_key);}

std::string	Channel::getTopic(void) const { return (this->_topic);}

int 		Channel::getUserFd(std::string name) {return (this->_users.at(name));}

bool		Channel::getIMode(void) const { return (this->_iMode); }

bool		Channel::getTMode(void) const { return (this->_tMode); }

bool		Channel::getKMode(void) const { return (this->_kMode); }

bool		Channel::getMaxClient (void) const { return (this->_maxNbClients); }

std::map<std::string, int>	Channel::getUsersArray(void) const { return (this->_users); }

void		Channel::setTopic(std::string &topic) { this->_topic = topic;}

void		Channel::setNumOfUsers(int i) {this->_nbclients += i;}

void		Channel::setIMode(int i) { this->_iMode = i; }

void		Channel::setTMode(int i) { this->_tMode = i; }

void		Channel::setKMode(int i) { this->_kMode = i; }

void		Channel::setKey(std::string key) { this->_key = key; }

void		Channel::setMaxClient (int i) { this->_maxNbClients = i; }

void		Channel::setUser(std::string name, int fd)
{
	if (this->userOnChannel(name) == 0)
	{
		this->_users.insert(std::make_pair(name, fd));
		this->setNumOfUsers(1);
	}
}

void		Channel::setOperator(std::string name)
{
	if (this->userOnChannel(name) && this->isOperator(name) == 0) //checks if user on channel & user not already operator
		this->_operators.insert (std::make_pair(name, getUserFd(name)));
}

void	Channel::removeOperator(std::string name)
{
	std::map<std::string, int>::iterator	it;

	it = this->_operators.find(name);
	if (it != this->_operators.end())
		this->_operators.erase(it);
}

void		Channel::removeClient(std::string name) // leave channel also there
{
	std::map<std::string, int>::iterator	it;

	it = this->_users.find(name);
	if (it != this->_users.end())
		this->_users.erase(it);
	
}

int			Channel::channelIsFull(void)
{
	if ((this->_maxNbClients - this->_nbclients) > 0)
		return (0); //channel not full
	return (1); //channel full
}

int			Channel::isOperator(std::string name)
{
	if (this->_operators.find(name) != this->_operators.end())
		return (1); // user is operator on channel
	return (0);
}

int			Channel::userOnChannel(std::string name) 
{
	if (_users.find(name) != _users.end())
		return (1); //user is on the channel
	return (0);
}

void Channel::broadcastMsg(std::string msg, Command *cmd)
{
	std::map<std::string, int>::iterator chItr;
	
	for(chItr = this->_users.begin(); chItr != this->_users.end(); chItr++)
		cmd->_srvr->sendResponse(chItr->second, msg);
}

void		Channel::leaveChannel(std::string name)
{
	std::map<std::string, int>::iterator chItr;

	chItr = this->_users.find(name);
	if (chItr != this->_users.end())
	{
		if (isOperator(name))
			this->removeOperator(name);
		this->_users.erase(chItr);
		this->setNumOfUsers(-1);
	}
}

void		Channel::setInvitedList(std::string name, int clientFd)
{
	if (this->isInvited(name) == 0)
		this->_invitedList.insert(std::make_pair(name, clientFd));
}

void		Channel::unsetInvitedList(std::string name)
{
	std::map<std::string, int>::iterator itr;

	itr = this->_invitedList.find(name);
	if (itr != this->_invitedList.end())
		this->_invitedList.erase(itr);
}

bool		Channel::isInvited(std::string name)
{
	std::map<std::string, int>::iterator itr;

	itr = this->_invitedList.find(name);
	if (itr != this->_invitedList.end())
		return (1); // invited
	return (0); // not invited
}

void	Channel::getUserList(std::string &str)
{
	std::map<std::string, int>::iterator itr;

	for (itr = _users.begin(); itr != _users.end(); itr++)
	{
		if (this->isOperator(itr->first) == 1)
			str += '@';
		str = str + itr->first;
		str += " ";
	}
	str.erase(str.length() -1);
}

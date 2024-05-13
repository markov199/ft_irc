
#include "Command.hpp"
#include "Server.hpp"

void	Command::executeTopic(int &clientFd)
{
	// ** syntax: TOPIC <channel> <:topic>
	std::vector<std::string>	vec = splitstr(this->_restMessage);
	std::string					clientNick = (this->_srvr)->getClient(clientFd)->getNickName();
	std::string					buff = "";
	std::string					prefix = (this->_srvr)->getClient(clientFd)->doPrefix();
	// ** checking errors
	if ((this->_srvr)->getClient(clientFd)->getIsAuthenticated() == 0)	{
		(this->_srvr)->sendResponse(clientFd, ERR_NOTREGISTERED(this->_host));
		return ;
	}
	
	if (vec.size() < 1 || vec.size() > 2) {
		(this->_srvr)->sendResponse(clientFd, ERR_NEEDMOREPARAMS(this->_command, this->_host));
		return ;
	}
	
	if ((this->_srvr)->channelExists(vec[0]) == 0)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NOSUCHCHANNEL(vec[0], this->_host));
		return ;
	}
	
	if (vec.size() == 1)
	{
		std::string topic = (this->_srvr)->getChannel(vec[0])->getTopic();
		if (!topic.empty()) //if topic is not empty
			(this->_srvr)->sendResponse(clientFd, RPL_TOPIC(prefix, clientNick, vec[0], topic));
		else
		{
			(this->_srvr)->sendResponse(clientFd, RPL_NOTOPIC(prefix, clientNick, vec[0]));
		}
		return ;
	}
	
	if (vec.size() == 2)
	{
		if ((this->_srvr)->getChannel(vec[0])->userOnChannel(clientNick) == 0)
		{
			(this->_srvr)->sendResponse(clientFd, ERR_NOTONCHANNEL(vec[0], this->_host)); // not in the channel
			return ;
		}
		if ((this->_srvr)->getChannel(vec[0])->getTMode() == 1) //restricted to operator
		{
			if ((this->_srvr)->getChannel(vec[0])->isOperator(clientNick) == 0) // not operator
			{
				(this->_srvr)->sendResponse(clientFd, ERR_CHANOPRIVSNEEDED(vec[0], this->_host));
				return ;
			}
		}
		 // not restricted to operator || user is operator in channel
		// if (vec[1][0] != ':')
		// {
		// 	(this->_srvr)->sendResponse(clientFd, ERR_NEEDMOREPARAMS(this->_command, this->_host));
		// 	return ;
		// }
		if (vec[1].length() > 1)
		{
				(this->_srvr)->getChannel(vec[0])->setTopic(vec[1]);
		}
		else
			(this->_srvr)->getChannel(vec[0])->setTopic(buff);
		std::map<int, Client *> clientMap = (this->_srvr)->getClientsArray();
		std::string prefix = clientMap.find(clientFd)->second->doPrefix();
		std::map<std::string, int> usersMap = (this->_srvr)->getChannel(vec[0])->getUsersArray();
		std::map<std::string, int>::iterator itUsersBegin = usersMap.begin();
		std::map<std::string, int>::iterator itUsersEnd = usersMap.end();
		while (itUsersBegin != itUsersEnd) 
		{ 
			(this->_srvr)->sendResponse(itUsersBegin->second, RPL_TOPIC(prefix, clientNick, vec[0], (this->_srvr)->getChannel(vec[0])->getTopic()));
			itUsersBegin++;
		}
	}
}

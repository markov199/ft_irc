
#include "Command.hpp"
#include "Server.hpp"

void	Command::executePass(int &clientFd)
{
	std::vector<std::string>	vec = splitstr(this->_restMessage);
	
	for (int i = 0; i < (int)this->_restMessage.length(); i++)
		if (isspace(this->_restMessage[i]) == 0)
		{
			if (i != 0)
				this->_restMessage.erase(0, i);
			break ;
		}
	if ((this->_srvr)->getClient(clientFd)->getPassAuth() == 1)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_ALREADYREGISTERED(this->_host));
		return ;
	}
	if (vec.size() == 0 || vec.size() > 1)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NEEDMOREPARAMS(this->_command, this->_host));
		return ;
	}
	if (vec[0] != (this->_srvr)->getPassword() || (vec.size() == 1 && (this->_restMessage).find_first_of(" \t") != std::string::npos))
	{
		(this->_srvr)->sendResponse(clientFd, ERR_PASSWDMISMATCH(this->_host));
		return ;
	}
	(this->_srvr)->getClient(clientFd)->setPassAuth(1);
}

void	Command::executeNick(int &clientFd)
{
	std::vector<std::string>			vec = splitstr(this->_restMessage);
	std::vector<std::string>::iterator	it = vec.begin();
	std::map<int, Client *>				clientMap = (this->_srvr)->getClientsArray();
	std::map<int, Client *>::iterator	cit;
	
	if ((this->_srvr)->getClient(clientFd)->getPassAuth() == 0)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NOTREGISTERED(this->_host));
		return ;
	}
	for (int i = 0; i < (int)this->_restMessage.length(); i++)
		if (isspace(this->_restMessage[i]) == 0)
		{
			if (i != 0)
				this->_restMessage.erase(0, i);
			break ;
		}
	if (vec.size() == 0)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NONICKNAMEGIVEN(this->_host));
		return ;
	}
	if (vec.size() > 1 || (*it).find_first_of(":#&!*?+,") != std::string::npos || (vec.size() == 1 \
	&& (this->_restMessage).find_first_of(" \t") != std::string::npos) || (*it).length() > 9)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_ERRONEUSNICKNAME(this->_host, this->_restMessage));
		return ;
	}
	for (cit = clientMap.begin(); cit != clientMap.end(); cit++)
	{
		if (cit->second->getNickName() == vec[0])
		{
			(this->_srvr)->sendResponse(clientFd, ERR_NICKNAMEINUSE(this->_host, vec[0]));
			return ;
		}
	}
	std::string oldnick = (this->_srvr)->getClient(clientFd)->getNickName();
	(this->_srvr)->getClient(clientFd)->setNickName(vec[0]);
	(this->_srvr)->getClient(clientFd)->setNickAuth(1);
	(this->_srvr)->sendResponse(clientFd, NICKNAMECHANGE(vec[0], oldnick));
	/* Here this last message has to be changed and done properly 		*/
	/* based if it's changing the nick or adding it for the first time	*/
}

void	Command::executeUser(int &clientFd)
{
	std::vector<std::string>			vec = splitstr(this->_restMessage);
	
	if ((this->_srvr)->getClient(clientFd)->getNickAuth() == 0)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NOTREGISTERED(this->_host));
		return ;
	}
	if ((this->_srvr)->getClient(clientFd)->getIsAuthenticated() == 1)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_ALREADYREGISTERED(this->_host));
		return ;
	}
	if (vec.size() != 4)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NEEDMOREPARAMS(this->_command, this->_host));
		return ;
	}
	if ((vec[1] != "0" && vec[1] != "*") || vec[2] != "*" )
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NEEDMOREPARAMS(this->_command, this->_host));
		(this->_srvr)->sendResponse(clientFd, "USER <username> * * :<realname>\r\n");
		return ;
	}
	if (vec[0].length() > 12)
		vec[0].erase(12, vec[0].length() - 12);
	(this->_srvr)->getClient(clientFd)->setUserName(vec[0]);
	(this->_srvr)->getClient(clientFd)->setUserAuth(1);
	(this->_srvr)->getClient(clientFd)->setIsAuthenticated(1);
	std::string	nick = (this->_srvr)->getClient(clientFd)->getNickName();
	(this->_srvr)->sendResponse(clientFd, RPL_WELCOME(nick, this->_host, (this->_srvr)->getClient(clientFd)->doPrefix()));
	(this->_srvr)->sendResponse(clientFd, RPL_YOURHOST(this->_host, nick));
	(this->_srvr)->sendResponse(clientFd, RPL_CREATED(this->_host, nick));
	(this->_srvr)->sendResponse(clientFd, RPL_MYINFO(this->_host, "i,t,k,o,l", nick));
}

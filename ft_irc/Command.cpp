
#include "Command.hpp"
#include "Server.hpp"

std::string	Command::_cmdArray[12] = {"PASS", "NICK", "USER", "JOIN", "PART", "KICK", "INVITE", "TOPIC", "MODE", "PRIVMSG", "QUIT", "PONG"};

Command::Command() { }

Command::Command(Command const &) {}

Command &Command::operator=(Command const &) { return *this; }

Command::~Command() { }
 
void	Command::parse(std::string msg, int &clientFd)
{
	int i;
	void (Command::*ptr[12]) (int &clientFd) = {&Command::executePass, &Command::executeNick, \
	&Command::executeUser, &Command::executeJoin, &Command::executePart, &Command::executeKick, &Command::executeInvite, \
	&Command::executeTopic, &Command::executeMode, &Command::executePrivmsg, &Command::executeQuit, &Command::executePong };
	
	for (i = 0; i < (int)msg.length(); i++)
	{
		if (isspace(msg[i]) != 0 || msg[i] == '\n')
			break ;
	}
	this->_command.append(msg, 0, i);
	if (i != (int)msg.length())
		this->_restMessage.append(msg.begin() + i + 1, msg.end());
	else
		this->_restMessage.clear();
	for (int i = 0; i < 12; i++)
	{
		if (this->compare(this->_command, this->_cmdArray[i]) == 0)
		{
			// if (i != 1)
			// {
			// 	if (this->_restMessage == ":") //this is to delete the : when commands like part are sent with no arguments in limechat
			// 	this->_restMessage.clear();
			// }
			// if (i == 3 && this->_restMessage == "#") // this is to remove the #
			// 	this->_restMessage.clear();
			(this->*ptr[i])(clientFd);
			return ;
		}
	}
	(this->_srvr)->sendResponse(clientFd, ERR_UNKNOWNCOMMAND(this->_command, this->_host));
}

void	Command::execute(int &clientFd, std::string msg, Server *srv)
{
	this->_srvr = srv;
	this->_host = (this->_srvr)->getHostName();
	
	if (!msg.empty() && isspace(msg[0]) == 0)
	{
		if (msg[0] == '/')
			msg.erase(0, 1);
		parse(msg, clientFd);
	}
	else
		(this->_srvr)->sendResponse(clientFd, ERR_UNKNOWNCOMMAND(this->_command, this->_host));
	this->_command.clear();
	this->_restMessage.clear();

}

void 	Command::executeQuit(int &clientFd)
{
	std::vector<std::string> vec = splitstr(this->_restMessage);

	if ((this->_srvr)->getClient(clientFd)->getIsAuthenticated() == 0)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NOTREGISTERED(this->_host));
		return ;
	}	
	std::string reason;
	if (vec.size() >= 1 && vec[0][0] == ':')
		reason = vec[0];
	else
		reason = "Leaving";
	
	std::map<int, Client *> clientMap = (this->_srvr)->getClientsArray();
	std::map<int, Client *>::iterator	it;
	std::string prefix = clientMap.find(clientFd)->second->doPrefix();

	(this->_srvr)->sendResponse(clientFd, RPL_QUIT(prefix, reason));
	std::cout <<">> QUIT command done on socket fd: " <<clientFd <<std::endl;				

	(this->_srvr)->removeClient(clientFd);
}

void 	Command::executePong(int &clientFd)
{
	(void)clientFd;
	return ;
}

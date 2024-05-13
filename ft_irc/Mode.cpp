
#include "Command.hpp"
#include "Server.hpp"

void 	Command::executeMode(int &clientFd)
{
	char	_modeArray[5] = {'i', 't', 'k', 'o', 'l'};
	void (Command::*ptr[5]) (int &clientFd, std::string &param, std::string &prefix) = {&Command::iMode, &Command::tMode, &Command::kMode, &Command::oMode, &Command::lMode};
	std::vector<std::string>	vec = splitstr(this->_restMessage);				
	std::string 				buff = "";
	std::string					prefix;
	
	this->_clientNick = (this->_srvr)->getClient(clientFd)->getNickName();
	if (this->modeErrors(clientFd, vec) == 1)
		return ;
	this->_channelName = vec[0];
	while (vec.size() > 1)
	{
		prefix.clear();
		prefix.insert(prefix.begin(), vec[1][0]);
		if (prefix[0] != '+' && prefix[0] != '-')
		{
			(this->_srvr)->sendResponse(clientFd, ERR_UNKNOWNMODE(prefix, vec[0]));
			return ;
		}
		int param = 2;
		int j = 1;
		while(j < (int)vec[1].length())
		{
			int i = 0;
			int count = 1;
					while(i < 5)
			{	
				if (vec[1][j] == '+' || vec[1][j] == '-')
				{
					prefix.insert(prefix.begin(), vec[1][j]);
					j++;
					count++;
				}
				if (vec[1][j] == _modeArray[i])
				{
					count++;
					if (i > 1)
					{
						if ((param >= (int)vec.size() || (prefix[0] == '-' && i != 2)) && i != 3)
							(this->*ptr[i])(clientFd, buff, prefix);
						else
						{
							(this->*ptr[i])(clientFd, vec[param], prefix);
							param++;
						}
					}
					else
						(this->*ptr[i])(clientFd, vec[0], prefix);
				}
				i++;
				if (i >= 5 && count == 1)
				{
					std::string temp = "";
					temp.insert(temp.begin(), vec[1][j]);
					(this->_srvr)->sendResponse(clientFd, ERR_UNKNOWNMODE(temp, vec[0]));
					temp.clear();
					count++;
				}
			}
			j++;
		}
		for (j = param; j >= 2; j--)
			vec.erase(vec.begin() + j - 1);
	}
}
//<#testlarachannel> lel-khou has changed mode: +s

void 	Command::iMode(int &clientFd, std::string &param, std::string &prefix)
{
	(void)param;
	std::string mode = prefix + "i";
	std::string nick = (this->_srvr)->getClient(clientFd)->getNickName();
	if (prefix[0] == '+')
		(this->_srvr)->getChannel(this->_channelName)->setIMode(1);
	else
		(this->_srvr)->getChannel(this->_channelName)->setIMode(0);
	(this->_srvr)->getChannel(this->_channelName)->broadcastMsg(RPL_MODE(mode, nick, this->_channelName), this);
}

void 	Command::tMode(int &clientFd, std::string &param, std::string &prefix)
{
	(void)param;
	std::string mode = prefix + "t";
	std::string nick = (this->_srvr)->getClient(clientFd)->getNickName();
	if (prefix[0] == '+')
		(this->_srvr)->getChannel(this->_channelName)->setTMode(1);
	else
		(this->_srvr)->getChannel(this->_channelName)->setTMode(0);
	(this->_srvr)->getChannel(this->_channelName)->broadcastMsg(RPL_MODE(mode, nick, this->_channelName), this);
}

void 	Command::kMode(int &clientFd, std::string &param, std::string &prefix)
{
	std::string mode = prefix + "k";
	std::string nick = (this->_srvr)->getClient(clientFd)->getNickName();
	if (param.empty() == 1)// && prefix == "+")
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NEEDMOREPARAMS(this->_command, this->_host));
		return ;
	}
	if (prefix[0] == '+')
	{
		if ((this->_srvr)->getChannel(this->_channelName)->getKMode() == 1) // check if key is already set
		{
			(this->_srvr)->sendResponse(clientFd, ERR_KEYSET(this->_channelName));
			return;
		}
		if (param.find_first_of(" \t:") != std::string::npos) // key contains space or \t or :
		{
			(this->_srvr)->sendResponse(clientFd, ERR_WRONGKEY(this->_channelName, this->_host));
			return ;
		}
		(this->_srvr)->getChannel(this->_channelName)->setKMode(1);
		(this->_srvr)->getChannel(this->_channelName)->setKey(param);
	}
	else
	{
		if ((this->_srvr)->getChannel(this->_channelName)->getKey() != param) // in case key provided for -k is not the correct key
		{
			(this->_srvr)->sendResponse(clientFd, ERR_WRONGKEY(this->_channelName, this->_host));
			return ;
		}
		(this->_srvr)->getChannel(this->_channelName)->setKMode(0);
		(this->_srvr)->getChannel(this->_channelName)->setKey("");
	}
	(this->_srvr)->getChannel(this->_channelName)->broadcastMsg(RPL_MODE(mode, nick, this->_channelName), this);
}

void 	Command::oMode(int &clientFd, std::string &param, std::string &prefix)
{
	std::string mode = prefix + "o";
	std::string nick = (this->_srvr)->getClient(clientFd)->getNickName();
	if (param.empty() == 1)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NEEDMOREPARAMS(this->_command, this->_host));
		return ;
	}
	if ((this->_srvr)->getChannel(this->_channelName)->userOnChannel(param) == 0)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_USERNOTINCHANNEL(param, this->_channelName, this->_host));
		return ;
	}
	if (prefix[0] == '+')
		(this->_srvr)->getChannel(this->_channelName)->setOperator(param);
	else
		(this->_srvr)->getChannel(this->_channelName)->removeOperator(param);
	(this->_srvr)->getChannel(this->_channelName)->broadcastMsg(RPL_MODE(mode, nick, this->_channelName), this);
}

void 	Command::lMode(int &clientFd, std::string &param, std::string &prefix)
{
	std::string mode = prefix + "l";
	std::string nick = (this->_srvr)->getClient(clientFd)->getNickName();
	int i;
	for (i = 0; i < (int)param.length(); i++)
		if (isdigit(param[i]) == 0)
			break;
	if (i < (int)param.length())
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NEEDMOREPARAMS(this->_command, this->_host));
		return ;
	}
	if (prefix[0] == '+')
		(this->_srvr)->getChannel(this->_channelName)->setMaxClient(atoi(param.c_str()));
	else
		(this->_srvr)->getChannel(this->_channelName)->setMaxClient(INT_MAX);
	(this->_srvr)->getChannel(this->_channelName)->broadcastMsg(RPL_MODE(mode, nick, this->_channelName), this);
}

int		Command::modeErrors(int &clientFd, std::vector<std::string> vec)
{
	if (vec.size() < 2 || vec[1].length() < 2)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NEEDMOREPARAMS(this->_command, this->_host));
		return (1);
	}
	if ((this->_srvr)->getClient(clientFd)->getIsAuthenticated() == 0)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NOTREGISTERED(this->_host));
		return (1);
	}
	if ((this->_srvr)->channelExists(vec[0]) == 0)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NOSUCHCHANNEL(vec[0], this->_host));
		return (1);
	}
	if ((this->_srvr)->getChannel(vec[0])->userOnChannel(this->_clientNick) == 0)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_USERNOTINCHANNEL(this->_clientNick, vec[0], this->_host));
		return (1);
	}
	if ((this->_srvr)->getChannel(vec[0])->isOperator(this->_clientNick) == 0)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_CHANOPRIVSNEEDED(vec[0], this->_host));
		return (1);
	}
	return (0);
}


#include "Command.hpp"
#include "Server.hpp"

void 	Command::executeJoin(int &clientFd)
{
	std::vector<std::string> cmdParams;
	std::vector<std::string> channels;
	std::vector<std::string> keys;
	std::map<std::string, Channel *> ChannelMap;
	std::map<std::string, Channel *>::iterator	chItr;
	std::string name;
	std::string prefix;
	std::string msg;
	Channel *chn;
	Client *user;
	std::string userNick;
	int j = 0;

	if ((this->_srvr)->getClient(clientFd)->getIsAuthenticated() == 0)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NOTREGISTERED(this->_host));
		return ;
	}	
	cmdParams = getCmdVector(this->_restMessage, ' ');
	if (cmdParams.empty())
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NEEDMOREPARAMS(this->_command, this->_host));
		return ;
	}
	channels = getCmdVector(cmdParams.at(0), ',');
	name = channels.at(0);
	if (name.compare("0") == 0)
	{
		channels = (this->_srvr)->getClient(clientFd)->getChannelList();
		user = (this->_srvr)->getClient(clientFd);
		prefix = (this->_srvr)->getClient(clientFd)->doPrefix();
		for (int i = 0; i < (int)channels.size(); i++)
		{
			chn = (this->_srvr)->getChannel(channels.at(i));
			chn->broadcastMsg(RPL_PART(prefix, chn->getName(), ""), this);
			chn->leaveChannel(user->getNickName());
			user->unsetChannelList(chn->getName());
			if (chn->getNumOfClients() == 0)
				(this->_srvr)->removeChannel(chn->getName());
		}
		return ;
	}
	if (cmdParams.size() > 1)
		keys = getCmdVector(cmdParams.at(1), ',');
	for (int i= 0; i < ((int)channels.size()); i++)
	{
		ChannelMap = (this->_srvr)->getChannelsArray();
		name = channels.at(i);
		Client *client = (this->_srvr)->getClient(clientFd);
		userNick = client->getNickName();
		if (ChannelMap.size() == 0 || (this->_srvr)->channelExists(name) == 0) // creating new channels
		{
			if (!keys.empty() && j < (int)keys.size())
			{
				if (keys[j].find_first_of(" \t:") != std::string::npos)
				{
					(this->_srvr)->sendResponse(clientFd, ERR_BADCHANNELKEY(name));
					j++;
					continue ;
				}
			}
			if (name[0] == '#' && name.length() > 1 && name.find_first_of("# ,:", 1) == std::string::npos) // this is to check join # test - join #test#test - join test
			{
				Channel *newChannel = new Channel(name, clientFd, userNick);
				(this->_srvr)->_channelsArray.insert(std::make_pair(name, newChannel)); // in server
				(this->_srvr)->getClient(clientFd)->setChannelList(name); // update channel list in client
				// no need to setNumofUsers because it is set to 1 on construction
				prefix = client->doPrefix();
				newChannel->broadcastMsg(RPL_JOIN(prefix, name), this);
				prefix = this->_host;
				std::string	list;
				newChannel->getUserList(list);
				(this->_srvr)->sendResponse(clientFd, RPL_TOPIC(prefix, userNick, name, ""));
				(this->_srvr)->sendResponse(clientFd, RPL_NAMREPLY(prefix, userNick, name, list));
				(this->_srvr)->sendResponse(clientFd, RPL_ENDOFNAMES(prefix, userNick, name));
						std::cout <<"Channel created" << newChannel->getName() << std::endl;
				if (!keys.empty() && j < (int)keys.size())
				{
					newChannel->setKey(keys.at(j));
					newChannel->setKMode(1);
					j++;
				}
			}
			else // this is in case join # test or join test without #
			{
				(this->_srvr)->sendResponse(clientFd, ERR_NOSUCHCHANNEL(name, this->_host));
				if (j < (int)keys.size())
					j++;
				continue ;
			}
		}
		else // not a new channel
		{
			chn = (this->_srvr)->getChannel(name);
			if (chn->userOnChannel(userNick) == 1)
			{
				(this->_srvr)->sendResponse(clientFd, ERR_USERONCHANNEL(userNick, chn->getName(), this->_host));
				continue ;
			}
			if (chn->getIMode() == 1) // if invite only
			{
				if (chn->isInvited(userNick) == 0)
				{
					(this->_srvr)->sendResponse(clientFd, ERR_INVITEONLYCHAN(name));
					continue ;
				}
			}
			if (chn->channelIsFull() == 1)
			{
				(this->_srvr)->sendResponse(clientFd, ERR_CHANNELISFULL(chn->getName()));
				continue ;
			}
			if (chn->getKMode() == 0) // no key
			{
				(this->_srvr)->getClient(clientFd)->setChannelList(name); // update channel list in client
				chn->setUser(userNick, clientFd);
				prefix = client->doPrefix();
				chn->broadcastMsg(RPL_JOIN(prefix, name), this);
				prefix = this->_host;
				(this->_srvr)->sendResponse(clientFd, RPL_TOPIC(prefix, userNick, name, chn->getTopic()));
				std::string userList;
				chn->getUserList(userList);				
				(this->_srvr)->sendResponse(clientFd, RPL_NAMREPLY(prefix, userNick, name, userList));
				(this->_srvr)->sendResponse(clientFd, RPL_ENDOFNAMES(prefix, userNick, name));

			}
			else if (chn->getKMode() == 1) // there is key
			{
				if (j < (int)keys.size() && chn->getKey() == keys.at(j))
				{
					(this->_srvr)->getClient(clientFd)->setChannelList(name); // update channel list in client
					chn->setUser(userNick, clientFd);
					prefix = client->doPrefix();
					chn->broadcastMsg(RPL_JOIN(prefix, name), this);
					prefix = this->_host;
					std::string topic = chn->getTopic();
					(this->_srvr)->sendResponse(clientFd, RPL_TOPIC(prefix, userNick, name, topic));
					std::string userList;
					chn->getUserList(userList);				
					(this->_srvr)->sendResponse(clientFd, RPL_NAMREPLY(prefix, userNick, name, userList));
					(this->_srvr)->sendResponse(clientFd, RPL_ENDOFNAMES(prefix, userNick, name));
					j++;
				}
				else 
				{
					(this->_srvr)->sendResponse(clientFd, ERR_BADCHANNELKEY(chn->getName()));
					j++;
					continue ;
				}
			}
		} 
	}
}

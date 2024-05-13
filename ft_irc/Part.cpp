
#include "Command.hpp"
#include "Server.hpp"

void 	Command::executePart(int &clientFd)
{
	std::vector<std::string> cmdParams;
	std::vector<std::string> channelArray;
	Channel *channel;
	std::string partMsg = "";
	Client *user = (this->_srvr)->getClient(clientFd);

	if (user->getIsAuthenticated() == 0)
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
	channelArray = getCmdVector(cmdParams.at(0), ',');
	if (cmdParams.size() > 1)
	{
		// if (cmdParams.at(1)[0] == ':')
		partMsg =  cmdParams.at(1);
		// else partMsg = "";
	}
	for (int i = 0; i < (int)channelArray.size(); i++)
	{
		if ((this->_srvr)->channelExists(channelArray[i]) == 0)
		{
			(this->_srvr)->sendResponse(clientFd, ERR_NOSUCHCHANNEL(channelArray[i], this->_host));
			continue ;
		}
		channel = (this->_srvr)->getChannel(channelArray.at(i));
		if (channel->userOnChannel(user->getNickName()) == 1)
		{
			std::string prefix = user->doPrefix();
			std::string channelName = channel->getName();
			if (partMsg.empty() == 1)
				partMsg = user->getNickName();
			channel->broadcastMsg(RPL_PART(prefix, channelName, partMsg), this);
			channel->leaveChannel(user->getNickName());
			user->unsetChannelList(channel->getName());
			if (channel->getNumOfClients() == 0)
				(this->_srvr)->removeChannel(channelName);
		}
		else {
			(this->_srvr)->sendResponse(clientFd, ERR_USERNOTINCHANNEL(user->getNickName(), channel->getName(), this->_host));
		}
	}
}

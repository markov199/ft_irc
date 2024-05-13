
#include "Command.hpp"
#include "Server.hpp"

void	Command::executeKick(int &clientFd)
{
	std::vector<std::string> cmdParams;
	std::vector<std::string> channels;
	std::vector<std::string> users;
	std::string comment;
	std::string nick = (this->_srvr)->getClient(clientFd)->getNickName();
	std::string prefix = (this->_srvr)->getClient(clientFd)->doPrefix();

	if ((this->_srvr)->getClient(clientFd)->getIsAuthenticated() == 0)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NOTREGISTERED(this->_host));
		return ;
	}	
	cmdParams = this->getCmdVector(this->_restMessage, ' ');
	if (cmdParams.size() <= 1){
		(this->_srvr)->sendResponse(clientFd, ERR_NEEDMOREPARAMS(this->_command, this->_host));
		return ;
	}
	if (cmdParams.size() >= 3 && cmdParams.at(2)[0] == ':')
		comment = cmdParams.at(2);
	else 
		comment = "";
	channels = this->getCmdVector(cmdParams.at(0), ',');
	users = this->getCmdVector(cmdParams.at(1), ',');
	if (channels.size() == 1)
	{
		if ((this->_srvr)->channelExists(channels[0]) == 0)
		{
			(this->_srvr)->sendResponse(clientFd, ERR_NOSUCHCHANNEL(channels[0], this->_host));
			return ;
		}
		if ((this->_srvr)->getClient(clientFd)->isInChannel(channels[0]) == 0) // client who is kicking is not even in channel
		{
				(this->_srvr)->sendResponse(clientFd, ERR_NOTONCHANNEL(channels[0], this->_host));
				return ;
		}
		if ((this->_srvr)->getChannel(channels[0])->isOperator(nick) == 0) //user requesting kick is not operator
		{
				(this->_srvr)->sendResponse(clientFd, ERR_CHANOPRIVSNEEDED(channels[0], this->_host));
				return ;
		}
		Channel *channelPtr = (this->_srvr)->getChannel(channels.at(0));
		for (int i = 0; i < (int)users.size(); i++){
			Client *channelUser = (this->_srvr)->getClient(users.at(i));
			if (channelUser == NULL) // check if user exists
				(this->_srvr)->sendResponse(clientFd, ERR_NOSUCHNICK(users.at(i), this->_host));
			else if(channelUser->isInChannel(channels[0]) == 0) // check if user is on channel
				(this->_srvr)->sendResponse(clientFd, ERR_USERNOTINCHANNEL(users.at(i), channels.at(0), this->_host));
			else if ((this->_srvr)->getChannel(channels[0])->isOperator(users[i]) == 1) /* user being kicked is operator NEED TO CHECK WHAT MESSAGE TO PUT HERE */
			{
				(this->_srvr)->sendResponse(clientFd, ERR_CHANOP(channels[0], this->_host));
				return ;
			}
			else
			{
				channelPtr->broadcastMsg(RPL_KICK(users[i], prefix, channels[0], comment), this);
				channelPtr->leaveChannel(users.at(i));
				channelUser->unsetChannelList(channelPtr->getName());
			}
		}
	}
	else if (channels.size() == users.size())
	{
		for (int i = 0; i < (int)channels.size(); i++)
		{
			if ((this->_srvr)->channelExists(channels[i]) == 0)
			{
				(this->_srvr)->sendResponse(clientFd, ERR_NOSUCHCHANNEL(channels[i], this->_host));
				continue ;
			}
			if ((this->_srvr)->getClient(clientFd)->isInChannel(channels[i]) == 0) // client who is kicking is not even in channel
			{
				(this->_srvr)->sendResponse(clientFd, ERR_NOTONCHANNEL(channels[i], this->_host));
				return ;
			}
			if ((this->_srvr)->getChannel(channels[i])->isOperator(nick) == 0) //user requesting kick is not operator
			{
				(this->_srvr)->sendResponse(clientFd, ERR_CHANOPRIVSNEEDED(channels[i], this->_host));
				return ;
			}
			Channel *channelPtr = (this->_srvr)->getChannel(channels.at(i));
			Client *channelUser = (this->_srvr)->getClient(users.at(i));
			if(channelUser == NULL) //check if user exists
				(this->_srvr)->sendResponse(clientFd, ERR_NOSUCHNICK(users.at(i), this->_host));
			else if(channelUser->isInChannel(channels[i]) == 0) //check if user is on channel
				(this->_srvr)->sendResponse(clientFd, ERR_USERNOTINCHANNEL(users[i], channels[i], this->_host));
			else if ((this->_srvr)->getChannel(channels[i])->isOperator(users[i]) == 1) /* user being kicked is operator NEED TO CHECK WHAT MESSAGE TO PUT HERE */
			{
				(this->_srvr)->sendResponse(clientFd, ERR_CHANOP(channels[i], this->_host));
				continue ;
			}
			else
			{
				channelPtr->broadcastMsg(RPL_KICK(users[i], prefix, channels[i], comment), this);
				channelPtr->leaveChannel(users.at(i));
				channelUser->unsetChannelList(channelPtr->getName());
			}
		}
	}
	else // in cases like kick #test,#test1 lara
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NEEDMOREPARAMS(this->_command, this->_host));
		return ;
	}

}

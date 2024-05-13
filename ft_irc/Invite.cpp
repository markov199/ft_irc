
#include "Command.hpp"
#include "Server.hpp"

void 	Command::executeInvite(int &clientFd)
{
	Client *client;
	std::vector<std::string> cmdParams;
	std::string channelname;
	std::string	userNick;
	Channel *chnPtr;
	
	if ((this->_srvr)->getClient(clientFd)->getIsAuthenticated() == 0)
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NOTREGISTERED(this->_host));
		return ;
	}
	cmdParams =  this->getCmdVector(this->_restMessage, ' ');
	if (cmdParams.size() <= 1){
		(this->_srvr)->sendResponse(clientFd, ERR_NEEDMOREPARAMS(this->_command, this->_host));
		return ;
	}
	userNick = cmdParams.at(0);
	channelname = cmdParams.at(1);
	if ((this->_srvr)->getClient(userNick) == NULL) // if userNick doesn't exist
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NOSUCHNICK(userNick, this->_host));
		return ;
	}
	if ((this->_srvr)->channelExists(channelname) == 0) // if channel doesn't exist
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NOSUCHCHANNEL(channelname, this->_host));
		return ;
	}
	chnPtr = (this->_srvr)->getChannel(cmdParams.at(1));
	client = (this->_srvr)->getClient(clientFd);
	if (chnPtr->userOnChannel(client->getNickName()) == 0) // if client is not on channel he is inviting to
	{
		(this->_srvr)->sendResponse(clientFd, ERR_NOTONCHANNEL(chnPtr->getName(), this->_host));
		return ;
	}
	if (chnPtr->getIMode() == 1 && chnPtr->isOperator(client->getNickName()) == 0) //invite only and client not operator
	{
			(this->_srvr)->sendResponse(clientFd, ERR_CHANOPRIVSNEEDED(chnPtr->getName(), this->_host));
			return ;
	}
	if (chnPtr->userOnChannel(userNick)) //person invited is already on channel
	{
		(this->_srvr)->sendResponse(clientFd, ERR_USERONCHANNEL(userNick, chnPtr->getName(), this->_host));
		return ;
	}
	chnPtr->setInvitedList(userNick, (this->_srvr)->getClient(userNick)->getFd());
	(this->_srvr)->sendResponse(clientFd, RPL_INVITING(chnPtr->getName(), userNick, this->_host));
	(this->_srvr)->sendResponse((this->_srvr)->getClient(userNick)->getFd(), RPL_INVITING(chnPtr->getName(), userNick, client->doPrefix()));
}

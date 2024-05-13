
#include "Command.hpp"
#include "Server.hpp"

void 	Command::executePrivmsg(int &clientFd)
{
	// syntax: PRIVMSG <target: nick or channelname> <:message>
	std::vector<std::string> vec = splitstr(this->_restMessage);
	
	// checking errors
	if ((this->_srvr)->getClient(clientFd)->getIsAuthenticated() == 0)	{
		(this->_srvr)->sendResponse(clientFd, ERR_NOTREGISTERED(this->_host));
		return ;
	}
	if (vec.size() == 0) {
		(this->_srvr)->sendResponse(clientFd, ERR_NORECIPIENT(this->_command, _host));
		return ;
	}
	if (vec.size() == 1) {
		if (vec[0][0] == ':')
			(this->_srvr)->sendResponse(clientFd, ERR_NORECIPIENT(this->_command, _host));
		else
			(this->_srvr)->sendResponse(clientFd, ERR_NOTEXTTOSEND(_host));
		return ;
	}
	if (vec.size() > 2) {
		(this->_srvr)->sendResponse(clientFd, ERR_TOOMANYTARGETS(vec[1], _host));
		return ;
	}
	// if (vec.size() == 2)&& vec[1][0] != ':') {
	// 	(this->_srvr)->sendResponse(clientFd, ERR_NOTEXTTOSEND(_host));
	// 	return ;
	// }

	
	std::string target = vec[0];
	std::string message = vec[1];
	if (message.at(0) == ':')
		message.erase(0, 1);
	std::string clientName = (this->_srvr)->getClient(clientFd)->getNickName();
	std::map<int, Client *> clientMap = (this->_srvr)->getClientsArray();
	std::map<std::string, Channel *> channelMap = (this->_srvr)->getChannelsArray();
	std::map<int, Client *>::iterator	it;
	std::map<std::string, Channel *>::iterator	it2;

	// check if target is a channel
	if (target[0] == '#')
	{
		for (it2 = channelMap.begin(); it2 != channelMap.end(); ++it2) {
			if (it2->second->getName() == target) { // found channel name
				if (it2->second->userOnChannel(clientName) == 0) {
					(this->_srvr)->sendResponse(clientFd, ERR_CANNOTSENDTOCHAN(target));
					return ;
				}
				std::string prefix = clientMap.find(clientFd)->second->doPrefix();
				std::map<std::string, int> usersMap = (this->_srvr)->getChannel(target)->getUsersArray();

				//now sending the message for all users inside this channel
				std::map<std::string, int>::iterator itUsersBegin = usersMap.begin();
				std::map<std::string, int>::iterator itUsersEnd = usersMap.end();
				while (itUsersBegin != itUsersEnd) { // broadcast to all
					if (itUsersBegin->second == clientFd) { // excluding sender
							itUsersBegin++;
							continue ;
						}
					(this->_srvr)->sendResponse(itUsersBegin->second, PRIVMSG(prefix, target, message));
					itUsersBegin++;
				}
				std::cout <<">> PRIVMSG sent to channel: " << target <<std::endl;
				return ;
			}
		}
		if (it2 == channelMap.end()) // channel not found
			(this->_srvr)->sendResponse(clientFd, ERR_NOSUCHNICK(target, _host));
	}

	else // target is a client
	{
		for (it = clientMap.begin(); it != clientMap.end(); ++it) {
			if (it->second->getNickName() == target) {
				std::string prefix = clientMap.find(clientFd)->second->doPrefix();
				(this->_srvr)->sendResponse(it->second->getFd(), PRIVMSG(prefix, target, message));
				std::cout <<">> PRIVMSG sent to client: " <<target <<std::endl;
				return ;
			}
		}
		if (it == clientMap.end())
			(this->_srvr)->sendResponse(clientFd, ERR_NOSUCHNICK(target, _host));
	}	
}

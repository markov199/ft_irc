
#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Server.hpp"
#include <iostream>
#include <map>
#include <climits>
class Client;
class Command;

class Channel {
private:
	std::string						_name; // this will be the name with prefix
	int								_nbclients; // this is just the nb of clients in the channel, might not be required as I don't see a limit to nb of clients per channel
	char							_prefix; // this is the first character from name
	std::string						_key;
	std::string						_topic; // only set once topic is set, otherwise a default "topic is not set" will be used
	bool							_iMode;
	bool							_tMode;
	bool							_kMode;
	int								_maxNbClients;
	std::map<std::string, int>		_operators; // map of clientsocket vs nicknames of operators of channel
	std::map<std::string, int>		_users; // map of clientsocket vs nicknames of users in channel
	std::map<std::string, int>		_invitedList;

	Channel();
	Channel(Channel const &);
	Channel	&operator=(Channel const &);

public:
	Channel(std::string &name, int &clientFd, std::string &nickname);
	~Channel();
	
	std::string	getName(void) const;
	char		getPrefix(void) const;
	int			getNumOfClients(void) const;
	std::string	getTopic(void) const;
	std::string	getKey(void) const;
	int 		getUserFd(std::string name);
	bool		getIMode(void) const;
	bool		getTMode(void) const;
	bool		getKMode(void) const;
	bool		getMaxClient (void) const;
	std::map<std::string, int>	getUsersArray(void) const;
	void	getUserList(std::string &str);
	
	void		setTopic(std::string &topic);
	void		setIMode(int i);
	void		setTMode(int i);
	void		setKMode(int i);
	void		setMaxClient (int i);
	void		setKey(std::string key);
	void		setNumOfUsers(int i); // i = 1 when adding and i = -1 when removing
	void		setUser(std::string name, int fd);
	void		setInvitedList(std::string client, int clientFd);
	void		unsetInvitedList(std::string client);
	bool		isInvited(std::string client);
	
	void		setOperator(std::string name);
	void		removeOperator(std::string name);
	void		removeClient(std::string name);
	int			channelIsFull(void);
	int			isOperator(std::string);
	int			userOnChannel(std::string name);
	void		leaveChannel(std::string name);
	void 		broadcastMsg(std::string msg, Command *cmd);
};

#endif

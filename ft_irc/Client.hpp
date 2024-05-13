
#pragma once

#include "Channel.hpp"
#include "Server.hpp"
#include <iostream>
#include <vector>
#include <deque>

class Client
{
    public:
        Client(int &fd, std::string &ip);
        ~Client();
		std::deque<std::string>	toBeSent;

        //actions of the client
        std::string doPrefix();

        //getters
		int				getFd() const;
		int				getPort() const;
        std::string	    getNickName() const;
        std::string		getUserName() const;
		std::string		getFullName() const;
		std::string		getIpAddressClient() const;
		bool			getIsAuthenticated() const;
		int				getIsRegistered() const; // we can delete after
		int				getIsOperator() const; // we can delete after
        std::string     getRequestMessage() const;
		bool			getPassAuth() const;
        bool			getNickAuth() const;
		bool            isInChannel(std::string channel);
		void			removeChannelList(std::string channelName);
		std::vector<std::string>    getChannelList();

        //setters
        void			setUserName(std::string userName);
		void			setNickName(std::string nickName);
		void			setFullName(std::string fullName);
		void			setIsAuthenticated(int i);
		void			setIsRegistered(int registered);
		void			setIsOperator(int isOperator);
        void            setRequestMessage(char message[]);
		void			setPassAuth(int i);
		void			setNickAuth(int i);
		void			setUserAuth(int i);
		void			setChannelList(std::string channelName);
		void			unsetChannelList(std::string channelName);
		int				compare(std::string s1, std::string s2) const;

    private:
        int             _fd; // client socket
        int             _port;
        std::string     _IpAddressClient;
        std::string     _nickName;
        std::string     _userName;
        std::string     _fullName;
        bool			_isAuthenticated;
		bool			_isRegistered;
		bool			_isOperator;
		bool			_passAuth;
		bool			_nickAuth;
		bool			_userAuth;
        std::string     _requestMessage;
        std::string 	_tempMessage;
		void			authenticate();
		
		std::vector<std::string> _myChannels;

        Client();
        Client(Client const &src);
        Client &operator=(Client const &rhs);
};

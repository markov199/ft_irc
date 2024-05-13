
#pragma once

#include <iostream>
#include "responses.hpp"
#include <vector>


class Server;

class Command
{
    public:
        Command();
        ~Command();

        //actions of Commands
        void parse(std::string msg, int &clientFd);
		void execute(int &clientFd, std::string msg, Server *srv);
		// functions to execute commands
		void	executeNick(int &clientFd);
		void	executeUser(int &clientFd);
		void	executeJoin(int &clientFd);
		void	executePrivmsg(int &clientFd);
		void	executeKick(int &clientFd);
		void	executeInvite(int &clientFd);
		void	executeTopic(int &clientFd);
		void	executeMode(int &clientFd);
		void	executePass(int &clientFd);
		void	executeQuit(int &clientFd);
		void	executePart(int &clientfd);
		void	executePong(int &clientfd);

		Server		*_srvr;

  	private:

        Command(Command const &);
        Command &operator=(Command const &);

		std::string					_command;
		std::string					_restMessage;
		static std::string			_cmdArray[12];
		std::string					_channelName;
		std::string					_clientNick; // initialized in executeMode
		std::string					_host;
		int							compare(std::string s1, std::string s2) const;
		std::vector<std::string>	splitstr(std::string &str);
		std::vector<std::string> 	getCmdVector(std::string message, const char c);
		std::string					trimAfterColon(std::string &str);
		void						iMode(int &clientFd, std::string &param, std::string &prefix);
		void						tMode(int &clientFd, std::string &param, std::string &prefix);
		void						kMode(int &clientFd, std::string &param, std::string &prefix);
		void						oMode(int &clientFd, std::string &param, std::string &prefix);
		void						lMode(int &clientFd, std::string &param, std::string &prefix);
		int							modeErrors(int &clientFd, std::vector<std::string> vec);		
};

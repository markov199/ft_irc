
#include "Server.hpp"
#include "Client.hpp"
#include "Command.hpp"
#include <csignal>

int	checkArgs(int ac, char *av[]);

int main(int ac, char *av[])
{
	if (checkArgs(ac, av) == -1) {
		return (-1);
	}

	int port = atoi(av[1]);
    std::string password = av[2];

	Server server(port, password);
	server.run();
	
	return (0);
}

int	checkArgs(int ac, char *av[])
{
	if (ac != 3) {
		std::cerr <<"Error! Usage-> \" ./ircserv <port> <password>\"" <<std::endl;
		return (-1);
	}
	
	std::string port = av[1];
	std::string pass = av[2];
	
	if (port.find_first_not_of("0123456789") != std::string::npos 
		|| port.length() > 5) {
		std::cerr <<"Error! Param <port> accepts only digits and must be between 1024 to 65535!" <<std::endl;
		return (-1);
	}

	int	portNumber = atoi(port.c_str());
	if (portNumber < 1024 || portNumber > 65535) {
		std::cerr <<"Error! Param <port> accepts only digits and must be between 1024 to 65535!" <<std::endl;
		return (-1);
	}

	if (pass.length() < 3 || pass.length() > 10) {
		std::cerr <<"Error! Param <password> must be between 3 to 10 printable characters!" <<std::endl;
		return (-1);
	}
	int i = 0;
	while (av[2][i]) {
		if (!isprint(av[2][i])) {
			std::cerr <<"Error! Param <password> accepts between 1 to 10 printable characters!" <<std::endl;
			return (-1);
		}
		if (av[2][i] == ' ') {
			std::cerr <<"Error! Param <password> do not accept ascii space char!" <<std::endl;
			return (-1);
		}
		i++;
	}        

	return (0);
}

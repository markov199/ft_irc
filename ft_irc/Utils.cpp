
#include "Command.hpp"
#include "Server.hpp"

int		Command::compare(std::string s1, std::string s2) const
{
	if (s1.length() != s2.length())
		return (1);
	for (int i = 0; i < (int)s1.length(); i++)
		if ((char)toupper(s1[i]) != (char)toupper(s2[i]))
			return (1);
	return (0);		
}

std::vector<std::string>	Command::splitstr(std::string &str)
{
	std::vector<std::string>	vec;
	int							i = 0;
	std::string					buff;
	
	if (str.empty())
		return (vec);
	while (i < (int)str.length())
	{
		if (isspace(str[i]) == 0)
			buff += str[i];
		else if(buff.length() > 0)
		{
			if (buff[0] == ':')
			{
				while (i < (int)str.length())
				{
					buff += str[i];
					i++;
				}
				break ;
			}
			vec.push_back(buff);
			buff = "";
		}
		i++;
	}
	if (!buff.empty())
        vec.push_back(buff);
    return (vec);
}

std::vector<std::string> Command::getCmdVector(std::string message, const char c)
{
	std::string temp;
	std::vector<std::string> cmdParams;
	int len = 0;

	temp = "";
	if (message.length() > 0)
	{
		for (int i = 0; i < (int)message.length(); i++)
		{
			if (len && (message[i] == c))
			{
				cmdParams.push_back(message.substr(i - len, len));
				len = 0;				
			}
			if (message[i] != c)
			{
				if (message[i] == ':' && len == 0)
				{
					cmdParams.push_back(message.substr(i));
					return (cmdParams);
				}
				len++;
			}
		}
		if (len)
			cmdParams.push_back(message.substr((message.length() - len), len));
	}
	return (cmdParams);
}

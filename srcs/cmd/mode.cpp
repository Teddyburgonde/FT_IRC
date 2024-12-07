#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Chanel.hpp"

void	modeCommand(int fd, Message &msg, std::vector<Chanel> &_chanel, std::vector<Client> &_clients)
{
	std::vector<Chanel>::iterator it_channel = _chanel.begin();
	std::string	argumentStr = msg.getArgument();
	const char	*argument = argumentStr.c_str();
	std::string	channelName;
	std::string	mode;
	int	i;

	//defini channelName et gere erreur de celui ci
	i = 0;
	while (argument[i] && argument[i] != ' ' && argument[i] != '\n')
		i++;
	channelName = std::string(argument, argument + i);
	if (!argument[0] || argument[0] != '#')
	{
		std::cout /*<< nom du serv*/ << "403 " << find_nickname_with_fd(fd, _clients) << " " << channelName << ":No such channel" << std::endl;
		// :irc.example.com 403 <nickname> <channel> :No such channel
	}

	//si je fais 'MODE #general i' donc joublie le + ou -
	//:irc.example.com 472 YourNickname i :is unknown mode char to me
	while (argument[i] && argument[i] == ' ' && argument[i] != '\n') //skip space
		i++;
}
#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Channel.hpp"


void Server::handlePrivMsg(int fd, Message &msg, std::vector<Channel> &_channel)
{
	int	index;

	index = 0;
	if (msg.getArgument().empty())
	{
    	std::cout << "Aucun argument dans le message" << std::endl;
    	return;
	}
	std::string recipient = get_next_argument(msg.getArgument().c_str(), index);
	if (recipient.empty())
	{
		std::string response = ERR_NORECIPIENT(std::string("Server"), "");
		send(fd, response.c_str(), response.size(), 0);
		return;
	}
	std::string message = get_next_argument(msg.getArgument().c_str(), index);
	message += "\n";
	if (message.empty())
	{
		std::string response = ERR_NOTEXTTOSEND(std::string("Server"));
		send(fd, response.c_str(), response.size(), 0);
		return;
	}
	if (recipient[0] == '#')
	{
		std::vector<Channel>::iterator it_channel_to_send = find_channel_with_name(recipient, _channel);
		if (it_channel_to_send == _channel.end())
		{
			std::string response = ERR_NOSUCHCHANNEL(recipient);
			send(fd, response.c_str(), response.size(), 0);
			return;
		}
		//si le userSender est pas dans le channel en question
		if (!is_user_in_chan(fd, it_channel_to_send->getUserInChannel()))
		{
			betterSend(ERR_NOTONCHANNEL(find_nickname_with_fd(fd, _clients), it_channel_to_send->getName()), fd);
			return;
		}
		it_channel_to_send->sendMessageToChannel(fd, message);
		return;
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getNickname() == recipient)
		{
			// Récupération des informations de l'émetteur
			std::string senderNickname = find_nickname_with_fd(fd, _clients);
			std::string senderUsername = find_username_with_fd(fd, _clients);

			std::string host = "localhost";
			
			// Construction du message IRC complet
			std::string ircMessage = formatIrcMessage(senderNickname, senderUsername, host, "PRIVMSG", recipient, message);
			send(_clients[i].getFd(), ircMessage.c_str(), ircMessage.size(), 0);
			return;
		}
	}
	std::string response = ERR_NOSUCHNICK(std::string("Server"), recipient);
	send(fd, response.c_str(), response.size(), 0);
}

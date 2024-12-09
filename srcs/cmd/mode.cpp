#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Chanel.hpp"

static void	find_mode(std::string &mode, std::vector<Chanel>::iterator it_channel, std::string nickname_of_sender, int fd)
{
	bool is_plus; //true if +, false if -

	if (mode[0] == '-')
		is_plus = false;
	else
		is_plus = true;
	if (mode[1] == 'i') //en gros, en fonction de la lettre donner, et de si c'est un plus ou moin, on init les bool mode de la class Channel
		(*it_channel).setModeI(is_plus);
	else if (mode[1] == 't')
		(*it_channel).setModeT(is_plus);
	else if (mode[1] == 'k')
		(*it_channel).setModeK(is_plus);
	else if (mode[1] == 'o')
		(*it_channel).setModeO(is_plus);
	else if (mode[1] == 'l')
		(*it_channel).setModeL(is_plus);
	else
	{
		std::string error /*<< nom du serv*/ = "472 " + nickname_of_sender + mode[1] + " :is unknown mode char to me";
		send(fd, error.c_str(), error.size(), 0);
	}
}

void	modeCommand(int fd, Message &msg, std::vector<Chanel> &_chanel, std::vector<Client> &_clients)
{
	std::vector<Chanel>::iterator it_channel = _chanel.begin();
	std::string	argumentStr = msg.getArgument();
	const char	*argument = argumentStr.c_str();
	std::string	channelName;
	std::string	mode;
	std::string nickname_of_sender;
	int	i;
	int	f;

	//defini channelName et gere erreur de celui ci
	nickname_of_sender = find_nickname_with_fd(fd, _clients);
	i = 0;
	while (argument[i] && argument[i] != ' ' && argument[i] != '\n')
		i++;
	channelName = std::string(argument, argument + i);
	it_channel = find_channel_with_name(channelName, _chanel); //it_channel = le bon channel
	if (!argument[0] || argument[0] != '#' || it_channel == _chanel.end()) //donc si le bon channel existe pas, erreur
	{
		std::string error = "403 " + nickname_of_sender + " " = channelName + ":No such channel";
		send(fd, error.c_str(), error.size(), 0);
		return;
	}
	if (is_user_in_chan(fd, (*it_channel).getOperatorUser()) == false)//check si le user qui a fais la commande est opm donc check si dans la list d'op du chan ya le user.
	{
		std::string error /*<< nom du serv*/ = "482 " + nickname_of_sender + " " + channelName + " :You're not channel operator";
		send(fd, error.c_str(), error.size(), 0);
		return;
	}
	//si je fais 'MODE #general i' donc joublie le + ou -
	while (argument[i] && argument[i] == ' ' && argument[i] != '\n') //skip space
		i++;
	//on recupere le +qqchose
	f = i;
	while (argument[f] && argument[f] != '\n' && argument[f] != ' ')//espace ???
		f++;
	mode = std::string(argument + i, argument + f);
	if ((mode[0] != '+' && mode[0] != '-') || (mode.size() != 2)) //si le premier char nest pas un + ou -
	{//erreur
		std::string error /*<< nom du serv*/ = "472 " + nickname_of_sender + " " + channelName + ":No such channel";
		send(fd, error.c_str(), error.size(), 0);
		return;
	}
	find_mode(mode, it_channel, nickname_of_sender, fd);
}

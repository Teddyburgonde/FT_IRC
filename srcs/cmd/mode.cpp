#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Chanel.hpp"

static void	handle_modeK(std::string argument, int fd, Message &msg, bool is_plus, Chanel it_channel)
{
	std::string password;
	int i;

	i = 0;
	while (argument[i] && argument[i] != ' ' && argument[i] != '\n')
		i++;
	password = std::string(argument.c_str(), argument.c_str() + i);
	if (password.empty())//error
	{
		std::string error = "461 " + msg.getCommand() + " :Not enough parameters";
		send(fd, error.c_str(), error.size(), 0);
		return;
	}
	it_channel.setModeK(is_plus);
	if (is_plus)
		it_channel.setPassword(argument);
	else
		it_channel.setPassword("");
}

static void	find_mode(std::string &mode, std::vector<Chanel>::iterator it_channel, std::string nickname_of_sender, int fd, std::string &argument, Message &msg)
{
	bool is_plus; //true if +, false if -

	if (mode[0] == '-')
		is_plus = false;
	else
		is_plus = true;
	if (mode[1] == 'i') //en gros, en fonction de la lettre donner, et de si c'est un plus ou moin, on init les bool mode de la class Channel
		(*it_channel).setModeI(is_plus);
	else if (mode[1] == 't') //good
		(*it_channel).setModeT(is_plus);
	else if (mode[1] == 'k') //set mdp au channel. mdp = argument
		handle_modeK(argument, fd, msg, is_plus, (*it_channel));
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
	std::string	str_string = msg.getArgument();
	const char	*line = str_string.c_str();
	std::string	channelName;
	std::string	mode;
	std::string nickname_of_sender;
	std::string argument; // argument a la fin de la lingne (line)
	int	i;
	int	f;

	//defini channelName et gere erreur de celui ci
	nickname_of_sender = find_nickname_with_fd(fd, _clients);
	i = 0;
	while (line[i] && line[i] != ' ' && line[i] != '\n')
		i++;
	channelName = std::string(line, line + i);
	it_channel = find_channel_with_name(channelName, _chanel); //it_channel = le bon channel
	if (!line[0] || line[0] != '#' || it_channel == _chanel.end()) //donc si le bon channel existe pas, erreur
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
	while (line[i] && line[i] == ' ' && line[i] != '\n') //skip space
		i++;
	//on recupere le +qqchose
	f = i;
	while (line[f] && line[f] != '\n' && line[f] != ' ') //on recupere tout le mode
		f++;
	mode = std::string(line + i, line + f);
	if ((mode[0] != '+' && mode[0] != '-') || (mode.size() != 2)) //si le premier char nest pas un + ou -
	{//erreur
		std::string error /*<< nom du serv*/ = "472 " + nickname_of_sender + " " + channelName + ":No such channel";
		send(fd, error.c_str(), error.size(), 0);
		return;
	}
	if (line[f] && line[f] != '\n') //si il reste des choses
	{
		while (line [f] && line[f] != '\n' && line[f] == ' ') //skip space
			f++;
		i = f; //i = debut de larg quon veut recup
		while (line[f] && line[f] != '\n') //recup arg
			f++;
		if (i != f)
			argument = std::string(line + i, line +f);
		else
			argument = "";
	}
	find_mode(mode, it_channel, nickname_of_sender, fd, argument, msg);
}

#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Chanel.hpp"

static void	handle_modeK(std::string argument, int fd, Message &msg, bool is_plus, Chanel it_channel, std::string nickname)
{
	std::string password;
	int 		i = 0;

	password = get_next_argument(argument.c_str(), i);
	if (password.empty())
	{
		send_error(ERR_NEEDMOREPARAMS(nickname, msg.getCommand()), fd);
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
		handle_modeK(argument, fd, msg, is_plus, (*it_channel), nickname_of_sender);
	else if (mode[1] == 'o')
		(*it_channel).setModeO(is_plus);
	else if (mode[1] == 'l')
		(*it_channel).setModeL(is_plus);
	else
		send_error(ERR_UNKNOWNMODE(nickname_of_sender, mode[1]), fd);
}

void	send_error(std::string error, int fd)
{
	send(fd, error.c_str(), error.size(), 0);
}

void	modeCommand(int fd, Message &msg, std::vector<Chanel> &_chanel, std::vector<Client> &_clients)
{
	std::vector<Chanel>::iterator it_channel = _chanel.begin();
	std::string	str_string = msg.getArgument();
	const char	*line = str_string.c_str();
	std::string nickname_of_sender;
	std::string argument = ""; // argument a la fin de la lingne (line)
	int	index = 0;

	//defini channelName et gere erreur de celui ci
	nickname_of_sender = find_nickname_with_fd(fd, _clients);
	std::string channelName = get_next_argument(line, index);
	if (channelName.empty() || channelName[0] != '#')
	{
		send_error(ERR_NOSUCHCHANNEL(channelName), fd);
		return;
	}
	it_channel = find_channel_with_name(channelName, _chanel); //it_channel = le bon channel
	if (it_channel == _chanel.end()) //donc si le bon channel existe pas, erreur
	{
		send_error(ERR_NOSUCHCHANNEL(channelName), fd);
		return;
	}
	if (is_user_in_chan(fd, (*it_channel).getOperatorUser()) == false)//check si le user qui a fais la commande est opm donc check si dans la list d'op du chan ya le user.
	{
		send_error(ERR_CHANOPRIVSNEEDED(nickname_of_sender, channelName), fd);
		return;
	}
	std::string mode = get_next_argument(line, index);
	if ((mode[0] != '+' && mode[0] != '-') || (mode.size() != 2)) //si le premier char nest pas un + ou -
	{
		send_error(ERR_NOSUCHCHANNEL(channelName), fd); //PAS SUR, LERREUR EST MAUVAIS ARGUMENT, PAS CHANNEL PAS TROUVE ????
		return;
	}
	if (line[index] && line[index] != '\n') //si il reste des choses
		argument = get_next_argument(line, index);
	find_mode(mode, it_channel, nickname_of_sender, fd, argument, msg);
}


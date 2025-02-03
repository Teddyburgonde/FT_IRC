#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Channel.hpp"

static void	handle_modeK(std::string argument, int fd, Message &msg, bool is_plus, Channel &it_channel, std::string nickname)
{
	std::string password;
	int			i = 0;

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

static void	handle_modeL(std::string argument, int fd, Message &msg, bool is_plus, Channel &it_channel, std::string nickname)
{
	std::string	nb_user_max;
	int i = 0;
	int number;

	nb_user_max = get_next_argument(argument.c_str(), i);
	if (nb_user_max.empty())
	{
		send_error(ERR_NEEDMOREPARAMS(nickname, msg.getCommand()), fd);
		return;
	}
	std::stringstream ss(nb_user_max);
	if (!(ss >> number) || !ss.eof()) //si on essaye de mettre ss dans number et que ca marche pas ou vide
	{
		send_error(ERR_TOOMUCHPARAMS(nickname, msg.getCommand()), fd);
		return;
	}
	it_channel.set_nb_user_max(number); // on defini le nb
	it_channel.setModeL(is_plus);
}

static void handle_modeO(std::string argument, int fd, Message &msg, bool is_plus, Channel &it_channel, std::string nickname, std::vector<Client> &_clients)
{
	int			i = 0;
	std::string	new_operator_name = get_next_argument(argument.c_str(), i);

	if(new_operator_name.empty())
	{
		send_error(ERR_NEEDMOREPARAMS(nickname, msg.getCommand()), fd);
		return;
	}
	int fd_new_op = find_fd_with_nickname(new_operator_name, _clients);
	if (fd_new_op == 0)
	{
		send_error(ERR_NOSUCHNICK(nickname, new_operator_name), fd);
		return;
	}
	if (!is_user_in_chan(find_fd_with_nickname(new_operator_name, _clients), it_channel.getUserInChannel()))
	{
		send_error(ERR_USERNOTINCHANNEL(nickname, new_operator_name, it_channel.getName()), fd);
		return;
	}
	it_channel.addUser(find_fd_with_nickname(new_operator_name, _clients), is_plus);
}

static void	find_mode(std::string &mode, std::vector<Channel>::iterator it_channel, std::string nickname_of_sender, int fd, std::string &argument, Message &msg, std::vector<Client> &_clients)
{
	bool is_plus; //true if +, false if -

	if (mode[0] == '-')
		is_plus = false;
	else
		is_plus = true;
	if (mode[1] == 'i') //en gros, en fonction de la lettre donner, et de si c'est un plus ou moin, on init les bool mode de la class Channel
		(*it_channel).setModeI(is_plus);
	else if (mode[1] == 't') //set le topic ?
		(*it_channel).setModeT(is_plus);
	else if (mode[1] == 'k') //set mdp au channel. mdp = argument
		handle_modeK(argument, fd, msg, is_plus, (*it_channel), nickname_of_sender);
	else if (mode[1] == 'o') //ajouter un nouvel operateur
		handle_modeO(argument, fd, msg, is_plus, (*it_channel), nickname_of_sender, _clients);
	else if (mode[1] == 'l') //nb user max
		handle_modeL(argument, fd, msg, is_plus, (*it_channel), nickname_of_sender);
	else
		send_error(ERR_UNKNOWNMODE(nickname_of_sender, mode[1]), fd);
}

void	modeCommand(int fd, Message &msg, std::vector<Channel> &_channel, std::vector<Client> &_clients)
{
	std::vector<Channel>::iterator it_channel = _channel.begin();
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
	it_channel = find_channel_with_name(channelName, _channel); //it_channel = le bon channel
	if (it_channel == _channel.end()) //donc si le bon channel existe pas, erreur
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
	if (mode.empty() || (mode[0] != '+' && mode[0] != '-') || (mode.size() != 2)) //si le premier char nest pas un + ou -
	{
		send_error(ERR_NEEDMOREPARAMS(nickname_of_sender, msg.getCommand()), fd);
		return;
	}
	if (line[index] && line[index] != '\n') //si il reste des choses
		argument = get_next_argument(line, index);
	find_mode(mode, it_channel, nickname_of_sender, fd, argument, msg, _clients);
}


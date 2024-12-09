#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Chanel.hpp"

int	find_fd_with_nickname(std::string &name, std::vector<Client> &_clients) // amettre dans utils ??
{
	std::vector<Client>::iterator	it = _clients.begin(); //iterator sur client

	while (it != _clients.end()) //on parcour tout les clients existant
	{
		if (name == (*it).getNickname()) //si le nom d'un client est le meme que celui donne en parametre
			return ((*it).getFd()); //on return le fd (int) du client.
		it++;
	}
	return (0);
}

//Pareil, a mettre dans utils ? Sert a trouver un nickname a partir d'un fd donne. est l'inverse de find_fd_with_name
std::string	find_nickname_with_fd(int fd, std::vector<Client> &_clients)
{
	std::vector<Client>::iterator	it = _clients.begin(); //iterator sur client

	while (it != _clients.end()) //on parcour tout les clients existant
	{
		if (fd == (*it).getFd()) //si le nom d'un client est le meme que celui donne en parametre
			return ((*it).getNickname()); //on return le fd (int) du client.
		it++;
	}
	return ("");
}
	

void	inviteCommand(int fd, Message &msg, std::vector<Chanel> &_chanel, std::vector<Client> &_clients)
{
	std::vector<Chanel>::iterator it_channel = _chanel.begin();
	int			fdUserToInvite;
	std::string	argumentStr = msg.getArgument();
	const char	*argument = argumentStr.c_str();
	std::string	channelName; //channel en question ou on veut inviter
	std::string name; //le nickname de la personne qu'on veut inviter
	int	i;
	int	f;

	i = 0;//recuperer le fd de la personne qu'on veut inviter et le mettre dans fdUserToInvite
	while (argument[i] && argument[i] != ' ' && argument[i] != '\n')
		i++;
	name = std::string(argument, argument + i); //name = le premier mot de msg
	fdUserToInvite = find_fd_with_nickname(name, _clients); //on recupere le fd correspondant au nom
	if (fdUserToInvite == 0) //error si le nickname est pas trouve
	{
		std::string error /*<< nom du serveur*/= "401 " + find_nickname_with_fd(fd, _clients) + " ";
		error = error + name + " :No such nick/channel";
		send(fd, error.c_str(), error.size(), 0);
		return;
	}
	while (argument[i] && argument[i] == ' ')//recuperer le channel dont on parle
		i++;
	f = i; //i est egale au debut de la string contenant le channel quon veut recup
	while (argument[f] && argument[f] != ' ' && argument[f] != '\n')
		f++; //on va a la fin de largument qui est le channel normalement
	channelName = std::string(argument + i, argument + f);
	if (!argument[i] || argument[i] != '#' || i == f) //error
	{
		std::string error/*<< nom du serveur*/= "479 " + find_nickname_with_fd(fd, _clients) + " ";
		error = error + channelName + " :Illegal channel name";
		send(fd, error.c_str(), error.size(), 0);
		return;
	}
	//definir it_client sur le bon channel
	while (it_channel != _chanel.end() && (*it_channel).getName() != channelName) //tant qu'on est pas sur le bon channel ou a la fin car on peut inviter dans un channel qui existe pas
		it_channel++;
	if (it_channel != _chanel.end())//dans le cas ou le channel existe
	{
		if ((*it_channel).getModeI() == true)
		{
			if (is_user_in_chan(fd, (*it_channel).getOperatorUser()) == false)
			{
				std::string nickname_of_sender = find_nickname_with_fd(fd, _clients);
				std::string error /*<< nom du serv*/ = "482 " + nickname_of_sender + " " + channelName + " :You're not channel operator";
				send(fd, error.c_str(), error.size(), 0);
				return;
			}
		}
		(*it_channel).setInvitedUser(fdUserToInvite);
	}
	std::string msgForUserToInvite =/*<< nom du serv*/ "341 " + name + " " + find_nickname_with_fd(fd, _clients) + channelName;
	send(fdUserToInvite, msgForUserToInvite.c_str(), msgForUserToInvite.size(), 0);
}

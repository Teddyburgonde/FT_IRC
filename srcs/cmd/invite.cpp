#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Chanel.hpp"

void	inviteCommand(int fd, Message &msg, std::vector<Chanel> &_chanel, std::vector<Client> &_clients)
{
	std::vector<Chanel>::iterator it_channel = _chanel.begin();
	int			fdUserToInvite;
	std::string	argument = msg.getArgument();
	std::string	channelName; //channel en question ou on veut inviter
	std::string nameInvite; //le nickname de la personne qu'on veut inviter
	std::string nick_of_sender = find_nickname_with_fd(fd, _clients); //le nom de fd, la personne qui fais la commande
	int	i;

	i = 0;//recuperer le fd de la personne qu'on veut inviter et le mettre dans fdUserToInvite
	nameInvite = get_next_argument(argument.c_str(), i);
	fdUserToInvite = find_fd_with_nickname(nameInvite, _clients); //on recupere le fd correspondant au nom
	if (fdUserToInvite == 0) //error si le nickname est pas trouve
	{
		send_error(ERR_NOSUCHNICK(nick_of_sender, nameInvite), fd);
		return;
	}
	channelName = get_next_argument(argument.c_str(), i);
	while (it_channel != _chanel.end() && (*it_channel).getName() != channelName) //tant qu'on est pas sur le bon channel ou a la fin car on peut inviter dans un channel qui existe pas
		it_channel++;
	if (channelName.empty() || channelName[0] != '#' || it_channel == _chanel.end())
	{
		send_error(ERR_NOSUCHCHANNEL(channelName), fd);
		return;
	}
	if (is_user_in_chan(fd, (*it_channel).getUserInChannel()) == false) //si pers qui fait cmd pas dans chan
	{
		send_error(ERR_USERNOTINCHANNEL(nick_of_sender, nick_of_sender, channelName), fd);
	}
	if (is_user_in_chan(fd, (*it_channel).getOperatorUser()) == false) //check si la pers qui a fait la cmd est op du chan
	{
		send_error(ERR_CHANOPRIVSNEEDED(nick_of_sender, channelName), fd);
		return;
	}
	(*it_channel).setInvitedUser(fdUserToInvite);
	//on envoie un message a la personne invitee
	std::string msgForUserToInvite = RPL_INVITESNDR(nick_of_sender, nameInvite, channelName);
	send(fdUserToInvite, msgForUserToInvite.c_str(), msgForUserToInvite.size(), 0);
}

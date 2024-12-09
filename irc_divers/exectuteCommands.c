void Server::execute_command( Request request)
{
	std::string cmd = request.get_cmd();
		
	if (cmd == "CAP")
		cap_command(request);
	else if (cmd == "PING")
		ping_command(request);
	else if (cmd == "PASS")
		pass_command(request);
	else if (cmd == "NICK")
		nick_command(request);
	else if (cmd == "USER")
		user_command(request);
	else if (cmd == "PRIVMSG")
		privmsg_command(request);
	else if (cmd == "QUIT")
		quit_command(request);
	else if (cmd == "JOIN" || cmd == "NAMES")
		join_names_command (request);
	else if (cmd == "GLOBOPS")
		globops_command(request);
	else if (cmd == "SHOWTIME")
		showtime_bot_command(request);
	else if (cmd == "LIST")
		list_command (request);
	else if (cmd == "TOPIC")
		topic_command (request);
	else if (cmd == "OPER")
		oper_command(request);
	else if (cmd == "PART")
		part_command (request);
	else if (cmd == "WHO")
		who_command (request);
	else if (cmd == "MODE")
		mode_command (request);
	else if (cmd == "KILL")
		kill_command(request);
	else if (cmd == "KICK")
		kick_command(request);
	else if (cmd == "NOTICE")
		notice_command(request);
	else if (cmd == "INVITE")
		invite_command(request);
	else
		send_message(SERVER_NAME " 421 " + request.get_userInChannel()->get_nickname() + " " + cmd + " :Unknown command", request.get_userInChannel()->get_fd());
}

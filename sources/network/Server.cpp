#include "../includes/Server.hpp"

void Server::WELCOME(ClientInfo *client)
{
	sender(client->client_fd, RPL_WELCOME(client->hostname, client->nickname, client->username));
	sender(client->client_fd, RPL_YOURHOST(client->hostname, client->nickname));
	sender(client->client_fd, RPL_CREATED(client->hostname, client->nickname));
}

Server::Server(std::string port, std::string password) : port(port), password(password)
{
	commandHandler = new CommandHandler(this);
}
Server::~Server()
{
	std::cout << RED << "-----------------------" << RESET << std::endl;
	std::cout << RED << "Server destroyed" << RESET << std::endl;
	delete commandHandler;
	close(socket_fd);
}

void Server::start()
{
	std::cout << std::endl;
	std::cout << GREEN << "----- Server started -----" << RESET << std::endl;
	std::cout << "Port: " << this->port << std::endl;
	std::cout << "Pass: " << this->password << std::endl;

	setup();

	while (poll(&poll_fd[0], poll_fd.size(), -1))
		MessageHandler();
}

void Server::setup()
{
	int opt = 1;
	this->socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
	fcntl(this->socket_fd, F_SETFL, O_NONBLOCK);

	if (this->socket_fd < 0)
		throw std::runtime_error("server socket creation failed");

	sockaddr_in6 server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin6_family = AF_INET6;
	server_addr.sin6_addr = in6addr_any;
	server_addr.sin6_port = htons(std::stoi(port));

	if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw std::runtime_error("socket option");

	if (bind(this->socket_fd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0)
		throw std::runtime_error("bind server socket failed");

	else
		std::cout << "Server is active now" << std::endl;

	if (listen(this->socket_fd, 5) < 0)
		throw std::runtime_error("listening error");

	poll_fd.resize(1);
	poll_fd[0].fd = socket_fd;
	poll_fd[0].events = POLLIN;
}

void Server::MessageHandler()
{
	for (size_t i = 0; i < poll_fd.size(); i++)
	{
		if (poll_fd[i].revents & POLLIN || poll_fd[i].revents & POLL_HUP)
		{
			if (poll_fd[i].revents & POLLHUP)
			{
				close(poll_fd[i].fd);

				for (size_t j = 0; j < channels.size(); ++j)
				{
					if (isClientInChannel(clients[i - 1], channels[j]))
					{
						PART partCommand(this, true);
						std::vector<std::string> partArgs;
						partArgs.push_back(channels[j].name);
						partCommand.execute(partArgs, &clients[i - 1]);
						partCommand.~PART();
					}
				}

				clients.erase(clients.begin() + i - 1);
				poll_fd.erase(poll_fd.begin() + i);
				std::cout << "Client disconnected" << std::endl;
				continue;
			}

			ClientAccept();
			if (i != 0)
			{
				std::string buffer = parseMessage(poll_fd[i].fd);

				for (size_t j = 0; j < clients.size(); j++)
				{
					if (clients[j].client_fd == poll_fd[i].fd)
					{
						commandHandler->invoke(buffer, &clients[j]);
						break;
					}
				}
			}
		}
	}
}
void Server::ClientAccept()
{
	if (poll_fd[0].revents & POLLIN)
	{
		sockaddr_in6 client_addr;
		socklen_t client_len = sizeof(client_addr);
		int client_fd = accept(this->socket_fd, reinterpret_cast<sockaddr *>(&client_addr), &client_len);
		if (client_fd < 0)
			throw std::runtime_error("accept error");
		else
		{
			ClientInfo client;
			ClientInit(client, client_fd);

			char ipstr[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, &(client_addr.sin6_addr), ipstr, sizeof(ipstr));

			sender(client.client_fd, "Welcome to the server, please enter your password: |COMMAND:PASS|\r\n");

			this->clients.push_back(client);
			std::cout << "New connection from " << ipstr << ":" << ntohs(client_addr.sin6_port) << std::endl;
			fcntl(client_fd, F_SETFL, O_NONBLOCK);

			poll_fd.resize(poll_fd.size() + 1);
			poll_fd[poll_fd.size() - 1].fd = client_fd;
			poll_fd[poll_fd.size() - 1].events = POLLIN;
		}
	}
}

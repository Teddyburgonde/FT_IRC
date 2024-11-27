#include "server.hpp"       // Inclusion des déclarations
#include <iostream>         // Pour afficher les messages
#include <cstring>          // Pour les opérations sur les chaînes de caractères
#include <cstdlib>          // Pour exit()
#include <sys/socket.h>     // Pour les fonctions de socket
#include <netinet/in.h>     // Pour sockaddr_in
#include <unistd.h>         // Pour close()
#include <poll.h>           // Pour gérer plusieurs sockets

// =======================
// Implémentation de Client
// =======================

// Constructeur de la classe Client
Client::Client(int fd) : fd(fd), nickname("Guest"), authenticated(false) {
    // Initialise un client avec un pseudo par défaut et non authentifié
}

// Destructeur de la classe Client
Client::~Client() {
    close(fd); // Ferme la socket du client lors de sa destruction
}

// =======================
// Implémentation de Server
// =======================

// Constructeur de la classe Server
Server::Server(int port, int buffer_size) : PORT(port), BUFFER_SIZE(buffer_size) {
    // Création du socket serveur
    server_fd = socket(AF_INET, SOCK_STREAM, 0); // IPv4, TCP
    if (server_fd < 0) {
        throw std::runtime_error("Socket creation failed"); // Gère une erreur de création
    }

    // Configuration de l'adresse du serveur
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;             // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;     // Accepte les connexions sur toutes les interfaces réseau
    server_addr.sin_port = htons(PORT);           // Port converti en format réseau

    // Liaison du socket à l'adresse et au port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        throw std::runtime_error("Bind failed"); // Erreur si la liaison échoue
    }

    // Mise en écoute
    if (listen(server_fd, 10) < 0) {
        throw std::runtime_error("Listen failed"); // Erreur si l'écoute échoue
    }

    std::cout << "Server started on port " << PORT << "\n";
}

// Destructeur de la classe Server
Server::~Server() {
    close(server_fd); // Ferme le socket serveur
    for (size_t i = 0; i < clients.size(); i++) {
        clients[i].~Client(); // Détruit les clients connectés
    }
}

// Méthode principale pour démarrer le serveur
void Server::start() {
    struct pollfd fds[100]; // Tableau pour surveiller jusqu'à 100 sockets
    fds[0].fd = server_fd;  // Le socket serveur
    fds[0].events = POLLIN; // Événement à surveiller : données prêtes à lire
    int nfds = 1; // Nombre total de sockets surveillées

    while (true) {
        int ret = poll(fds, nfds, -1); // Attente des événements
        if (ret < 0) {
            throw std::runtime_error("Poll error");
        }

        // Accepter une nouvelle connexion
        if (fds[0].revents & POLLIN) {
            int client_fd = accept(server_fd, NULL, NULL); // Accepte une connexion
            if (client_fd >= 0) {
                std::cout << "New client connected: " << client_fd << "\n";
                clients.push_back(Client(client_fd)); // Ajoute le client à la liste
                fds[nfds].fd = client_fd;            // Ajoute le client au tableau poll
                fds[nfds].events = POLLIN;           // Surveille les données à lire
                nfds++;
            }
        }

        // Gérer les événements sur les clients
        for (int i = 1; i < nfds; i++) {
            if (fds[i].revents & POLLIN) { // Données disponibles pour lecture
                char buffer[BUFFER_SIZE];
                int bytes_read = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0); // Lecture des données
                if (bytes_read <= 0) { // Déconnexion ou erreur
                    std::cout << "Client disconnected: " << fds[i].fd << "\n";
                    removeClient(fds[i].fd); // Supprime le client
                    fds[i] = fds[--nfds];
                } else {
                    buffer[bytes_read] = '\0'; // Terminer la chaîne reçue
                    handleCommand(std::string(buffer), fds[i].fd); // Traiter la commande reçue
                }
            }
        }
    }
}

// Diffuse un message à tous les clients sauf l'expéditeur
void Server::broadcastMessage(const std::string &message, int sender_fd) {
    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i].fd != sender_fd) { // N'envoie pas à l'expéditeur
            send(clients[i].fd, message.c_str(), message.size(), 0); // Envoie le message
        }
    }
}

// Traite une commande IRC
void Server::handleCommand(const std::string &command, int client_fd) {
    if (command.substr(0, 5) == "NICK ") { // Commande NICK
        std::string nickname = command.substr(5);
        for (size_t i = 0; i < clients.size(); i++) {
            if (clients[i].fd == client_fd) {
                clients[i].nickname = nickname;
                clients[i].authenticated = true;
                send(client_fd, "001 Welcome to the IRC server!\n", 32, 0);
                break;
            }
        }
    } else if (command.substr(0, 5) == "JOIN ") { // Commande JOIN
        std::string channel = command.substr(5);
        channels[channel].push_back(client_fd);
        std::string message = "JOIN " + channel + "\n";
        broadcastMessage(message, client_fd);
    } else if (command.substr(0, 8) == "PRIVMSG ") { // Commande PRIVMSG
        size_t space = command.find(' ', 8);
        if (space == std::string::npos) return;
        std::string target = command.substr(8, space - 8);
        std::string message = command.substr(space + 1);

        if (channels.find(target) != channels.end()) {
            for (size_t i = 0; i < channels[target].size(); i++) {
                send(channels[target][i], message.c_str(), message.size(), 0);
            }
        } else {
            for (size_t i = 0; i < clients.size(); i++) {
                if (clients[i].nickname == target) {
                    send(clients[i].fd, message.c_str(), message.size(), 0);
                    break;
                }
            }
        }
    }
}

// Supprime un client
void Server::removeClient(int client_fd) {
    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i].fd == client_fd) {
            clients.erase(clients.begin() + i); // Retire le client
            close(client_fd); // Ferme sa socket
            break;
        }
    }
}

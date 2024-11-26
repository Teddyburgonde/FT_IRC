#include "server.hpp"       // Inclusion du fichier d'en-tête contenant les déclarations
#include <iostream>         // Pour l'affichage (std::cout)
#include <cstring>          // Pour les opérations sur les chaînes de caractères (std::string)
#include <cstdlib>          // Pour les fonctions système comme exit()
#include <sys/socket.h>     // Pour les sockets
#include <netinet/in.h>     // Pour les structures d'adresses réseau (sockaddr_in)
#include <unistd.h>         // Pour close()

// =======================
// Implémentation de Client
// =======================

// Constructeur de la classe Client
Client::Client(int fd) : fd(fd), nickname("Guest"), authenticated(false) {
    // Initialise le client avec un descripteur de socket donné
    // Par défaut : nickname = "Guest", authenticated = false
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
        throw std::runtime_error("Socket creation failed"); // Erreur si la socket ne peut pas être créée
    }

    // Configuration de l'adresse du serveur
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;             // Utilisation de l'IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;     // Le serveur accepte les connexions depuis toutes les interfaces réseau
    server_addr.sin_port = htons(PORT);          // Port du serveur (converti en format réseau)

    // Liaison de la socket à l'adresse et au port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        throw std::runtime_error("Bind failed"); // Erreur si l'association échoue
    }

    // Mise en écoute de la socket pour accepter les connexions
    if (listen(server_fd, 10) < 0) {
        throw std::runtime_error("Listen failed"); // Erreur si l'écoute échoue
    }

    std::cout << "Server started on port " << PORT << "\n"; // Indique que le serveur est prêt
}

// Destructeur de la classe Server
Server::~Server() {
    close(server_fd); // Ferme le socket serveur
    for (size_t i = 0; i < clients.size(); i++) {
        clients[i].~Client(); // Ferme tous les sockets des clients connectés
    }
}

// Méthode principale pour démarrer le serveur
void Server::start() {
    struct pollfd fds[100]; // Tableau pour surveiller jusqu'à 100 sockets
    fds[0].fd = server_fd;  // Le socket serveur est surveillé en premier
    fds[0].events = POLLIN; // Événement à surveiller : données prêtes à lire

    int nfds = 1; // Nombre total de sockets surveillées (commence avec le socket serveur)

    while (true) { // Boucle infinie pour gérer les connexions et les messages
        int ret = poll(fds, nfds, -1); // Attend des événements sur les sockets
        if (ret < 0) {
            throw std::runtime_error("Poll error"); // Erreur si poll échoue
        }

        // Vérifie si une nouvelle connexion est disponible sur le socket serveur
        if (fds[0].revents & POLLIN) {
            int client_fd = accept(server_fd, NULL, NULL); // Accepte la nouvelle connexion
            if (client_fd >= 0) {
                std::cout << "New client connected: " << client_fd << "\n";
                clients.push_back(Client(client_fd)); // Ajoute le client à la liste
                fds[nfds].fd = client_fd;            // Ajoute le client au tableau poll
                fds[nfds].events = POLLIN;           // Surveiller les données prêtes à lire
                nfds++;                              // Incrémente le nombre total de sockets surveillées
            }
        }

        // Vérifie les événements sur les sockets des clients
        for (int i = 1; i < nfds; i++) {
            if (fds[i].revents & POLLIN) { // Si des données sont disponibles pour lecture
                char buffer[BUFFER_SIZE]; // Tampon pour stocker les données reçues
                int bytes_read = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0); // Lit les données du client
                if (bytes_read <= 0) { // Si le client s'est déconnecté ou erreur
                    std::cout << "Client disconnected: " << fds[i].fd << "\n";
                    removeClient(fds[i].fd); // Supprime le client de la liste
                    fds[i] = fds[--nfds];   // Retire le socket de la liste poll
                } else {
                    buffer[bytes_read] = '\0'; // Termine la chaîne reçue avec '\0'
                    handleCommand(std::string(buffer), fds[i].fd); // Traite la commande reçue
                }
            }
        }
    }
}

// Diffuse un message à tous les clients sauf l'expéditeur
void Server::broadcastMessage(const std::string &message, int sender_fd) {
    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i].fd != sender_fd) { // N'envoie pas le message à l'expéditeur
            send(clients[i].fd, message.c_str(), message.size(), 0); // Envoie le message
        }
    }
}

// Traite une commande envoyée par un client
void Server::handleCommand(const std::string &command, int client_fd) {
    if (command.substr(0, 5) == "NICK ") { // Commande pour définir un pseudo
        std::string nickname = command.substr(5); // Extrait le pseudo
        for (size_t i = 0; i < clients.size(); i++) {
            if (clients[i].fd == client_fd) { // Met à jour les informations du client
                clients[i].nickname = nickname;
                clients[i].authenticated = true;
                send(client_fd, "001 Welcome to the IRC server!\n", 32, 0); // Envoie un message de bienvenue
                break;
            }
        }
    } else if (command.substr(0, 5) == "JOIN ") { // Commande pour rejoindre un canal
        std::string channel = command.substr(5); // Extrait le nom du canal
        channels[channel].push_back(client_fd);  // Ajoute le client au canal
        std::string message = "JOIN " + channel + "\n";
        broadcastMessage(message, client_fd);    // Informe les autres membres du canal
    } else if (command.substr(0, 8) == "PRIVMSG ") { // Commande pour envoyer un message
        size_t space = command.find(' ', 8);
        if (space == std::string::npos) return;   // Vérifie si la commande est valide
        std::string target = command.substr(8, space - 8); // Extrait la cible
        std::string message = command.substr(space + 1);   // Extrait le message

        // Vérifie si la cible est un canal ou un utilisateur
        if (channels.find(target) != channels.end()) { // Si c'est un canal
            for (size_t i = 0; i < channels[target].size(); i++) {
                send(channels[target][i], message.c_str(), message.size(), 0); // Envoie aux membres du canal
            }
        } else { // Sinon, cherche un utilisateur
            for (size_t i = 0; i < clients.size(); i++) {
                if (clients[i].nickname == target) {
                    send(clients[i].fd, message.c_str(), message.size(), 0); // Envoie à l'utilisateur cible
                    break;
                }
            }
        }
    }
}

// Supprime un client de la liste et ferme sa socket
void Server::removeClient(int client_fd) {
    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i].fd == client_fd) {
            clients.erase(clients.begin() + i); // Retire le client de la liste
            close(client_fd); // Ferme la socket
            break;
        }
    }
}

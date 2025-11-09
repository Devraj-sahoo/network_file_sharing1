#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <dirent.h>
#include <fstream>

void xorEncrypt(char* data, int length, int key = 123) {
    for (int i = 0; i < length; i++) {
        data[i] ^= key;
    }
}

std::string getFileList() {
    std::string folder = "server_files";
    DIR *dir = opendir(folder.c_str());
    if (!dir) return "ERROR: server_files missing\n";

    struct dirent *entry;
    std::string fileList = "";

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0)
        {
            fileList += entry->d_name;
            fileList += "\n";
        }
    }
    closedir(dir);
    return fileList;
}

// ✅ Authentication Function
bool authenticateUser(int client_socket) {
    char buffer[1024];

    // Username
    memset(buffer, 0, 1024);
    read(client_socket, buffer, 1024);
    std::string username = buffer;

    // Password
    memset(buffer, 0, 1024);
    read(client_socket, buffer, 1024);
    std::string password = buffer;

    if (username == "admin" && password == "1234") {
        send(client_socket, "AUTH_OK", 7, 0);
        return true;
    } else {
        send(client_socket, "AUTH_FAIL", 9, 0);
        return false;
    }
}

// ✅ Send File (Download)
void sendFile(int client_socket) {
    // 1. Send file list
    std::string list = getFileList();
    send(client_socket, list.c_str(), list.size(), 0);

    // 2. Receive filename
    char buffer[2048] = {0};
    read(client_socket, buffer, 2048);
    std::string filename = buffer;

    std::string filepath = "server_files/" + filename;

    std::ifstream file(filepath, std::ios::binary);

    if (!file) {
        long err = -1;
        send(client_socket, &err, sizeof(err), 0);
        return;
    }

    // 3. File size
    file.seekg(0, std::ios::end);
    long filesize = file.tellg();
    file.seekg(0, std::ios::beg);

    send(client_socket, &filesize, sizeof(filesize), 0);

    // 4. Send file chunks (encrypted)
    char chunk[1024];
    while (!file.eof()) {
        file.read(chunk, sizeof(chunk));
        int bytes = file.gcount();
        xorEncrypt(chunk, bytes);
        send(client_socket, chunk, bytes, 0);
    }

    file.close();
    std::cout << "File sent successfully.\n";
}

// ✅ Receive File (Upload)
void receiveFile(int client_socket) {
    char buffer[1024] = {0};

    // 1. Receive filename
    read(client_socket, buffer, 1024);
    std::string filename = buffer;

    std::string savepath = "server_uploads/" + filename;
    std::ofstream outfile(savepath, std::ios::binary);

    // 2. Receive file size
    long filesize;
    read(client_socket, &filesize, sizeof(filesize));

    // 3. Receive file chunks (decrypt)
    long received = 0;
    char chunk[1024];

    while (received < filesize) {
        int bytes = read(client_socket, chunk, sizeof(chunk));
        xorEncrypt(chunk, bytes);
        outfile.write(chunk, bytes);
        received += bytes;
    }

    outfile.close();
    std::cout << "File uploaded to server successfully!\n";
}

// ✅ MAIN SERVER PROGRAM
int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, 1);

    std::cout << "Server waiting for client...\n";

    socklen_t addr_len = sizeof(server_addr);
    int client_socket = accept(server_socket, (sockaddr*)&server_addr, &addr_len);

    std::cout << "Client connected!\n";

    // ✅ Step 1: Authenticate
    if (!authenticateUser(client_socket)) {
        std::cout << "Authentication failed.\n";
        close(client_socket);
        close(server_socket);
        return 0;
    }
    std::cout << "Client authenticated.\n";

    // ✅ Step 2: Identify operation
    char choice[1024] = {0};
    read(client_socket, choice, 1024);

    if (strcmp(choice, "DOWNLOAD") == 0)
        sendFile(client_socket);
    else if (strcmp(choice, "UPLOAD") == 0)
        receiveFile(client_socket);

    close(client_socket);
    close(server_socket);
    return 0;
}

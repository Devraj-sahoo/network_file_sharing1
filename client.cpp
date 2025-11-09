#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <fstream>

void xorEncrypt(char* data, int length, int key = 123) {
    for (int i = 0; i < length; i++) {
        data[i] ^= key;
    }
}

// ✅ Authentication
void authenticate(int sock) {
    std::string username, password;

    std::cout << "Enter username: ";
    std::cin >> username;
    send(sock, username.c_str(), username.size(), 0);

    std::cout << "Enter password: ";
    std::cin >> password;
    send(sock, password.c_str(), password.size(), 0);

    char buffer[1024] = {0};
    read(sock, buffer, 1024);

    if (strcmp(buffer, "AUTH_FAIL") == 0) {
        std::cout << "❌ Authentication Failed\n";
        close(sock);
        exit(0);
    }
    std::cout << "✅ Authentication Successful\n";
}

// ✅ DOWNLOAD FUNCTION
void downloadFile(int sock) {
    char buffer[2048] = {0};
    read(sock, buffer, 2048);

    std::cout << "\nAvailable Files on Server:\n" << buffer << std::endl;

    std::string filename;
    std::cout << "Enter filename to download: ";
    std::cin >> filename;

    send(sock, filename.c_str(), filename.size(), 0);

    long filesize;
    read(sock, &filesize, sizeof(filesize));

    if (filesize <= 0) {
        std::cout << "❌ File not found on server.\n";
        return;
    }

    std::string outpath = "client_downloads/" + filename;
    std::ofstream outfile(outpath, std::ios::binary);

    long received = 0;
    char chunk[1024];

    while (received < filesize) {
        int bytes = read(sock, chunk, sizeof(chunk));
        xorEncrypt(chunk, bytes);
        outfile.write(chunk, bytes);
        received += bytes;
    }

    std::cout << "✅ File downloaded successfully!\n";
    outfile.close();
}

// ✅ UPLOAD FUNCTION
void uploadFile(int sock) {
    std::string filename;
    std::cout << "Enter file name to upload (from client folder): ";
    std::cin >> filename;

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "❌ File not found in client folder.\n";
        return;
    }

    send(sock, filename.c_str(), filename.size(), 0);

    file.seekg(0, std::ios::end);
    long filesize = file.tellg();
    file.seekg(0, std::ios::beg);

    send(sock, &filesize, sizeof(filesize), 0);

    char chunk[1024];
    while (!file.eof()) {
        file.read(chunk, sizeof(chunk));
        int bytes = file.gcount();
        xorEncrypt(chunk, bytes);
        send(sock, chunk, bytes, 0);
    }

    file.close();
    std::cout << "✅ File uploaded successfully!\n";
}

// ✅ MAIN CLIENT PROGRAM
int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));
    std::cout << "✅ Connected to server\n";

    // Step 1: Login
    authenticate(sock);

    // Step 2: Menu
    int choice;
    std::cout << "\n1. Download File\n2. Upload File\nEnter choice: ";
    std::cin >> choice;

    if (choice == 1) {
        send(sock, "DOWNLOAD", 8, 0);
        downloadFile(sock);
    }
    else if (choice == 2) {
        send(sock, "UPLOAD", 6, 0);
        uploadFile(sock);
    }

    close(sock);
    return 0;
}

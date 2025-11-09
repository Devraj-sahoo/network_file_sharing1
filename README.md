# 🖥️ Network File Sharing System (Client–Server Architecture)

## 📘 Overview
This project implements a **secure client–server file sharing system** using **C++ socket programming**.  
The system allows authenticated users to **upload and download files** securely over a TCP connection.  
It includes encryption, authentication, and modular folder organization — suitable for college-level networking projects.

---

## 🚀 Features
### ✅ Core Features
- **Server–Client Communication** via TCP sockets  
- **User Authentication** (username + password)  
- **File Listing** on the server  
- **File Download** (Server → Client)  
- **File Upload** (Client → Server)  
- **XOR Encryption** for secure data transfer  

### 🔐 Security
- Basic XOR-based encryption ensures data confidentiality  
- Password authentication prevents unauthorized access  

---

## 🧠 Learning Outcomes
This project demonstrates:
- C++ socket programming (POSIX API)
- Network communication (TCP/IP)
- File handling & I/O streams
- Simple encryption/decryption
- Secure bidirectional file transfer

---

## 🧩 Technologies Used
| Component | Technology |
|------------|-------------|
| Language | C++ |
| Protocol | TCP/IP |
| Encryption | XOR Cipher |
| OS | Linux / macOS |
| IDE | Visual Studio Code |

---

## 🗂️ Folder Structure

network_file_sharing/
│
├── server/
│ ├── server.cpp
│ ├── Makefile
│ ├── server_files/ # Files available for download
│ └── server_uploads/ # Uploaded files saved here
│
├── client/
│ ├── client.cpp
│ ├── Makefile
│ └── client_downloads/ # Files downloaded from server
│
├── screenshots/ # For project screenshots (optional)
│ ├── server_running.png
│ ├── client_connected.png
│ ├── file_upload.png
│ └── file_download.png
│
└── README.md


---

## ⚙️ Setup & Installation

### 1️⃣ Clone or Create the Project
```bash
mkdir network_file_sharing
cd network_file_sharing

2️⃣ Open the Folder in VS Code
code .

3️⃣ Build the Server
cd server
make

4️⃣ Build the Client
cd ../client
make

▶️ Running the Project
🖥️ Start the Server

In VS Code Terminal 1:

cd server
./server


Output:

Server waiting for client...

💻 Start the Client

In VS Code Terminal 2:

cd client
./client

🔑 Authentication
Field	Default Value
Username	admin
Password	1234

✅ On successful login, the client gains access to upload and download operations.
❌ Invalid credentials disconnect the client.

📁 File Operations
📥 Download from Server

Client views file list from server_files/

Selects a file name (e.g., file1.txt)

Server encrypts and sends the file

Client decrypts and saves file in client_downloads/

📤 Upload to Server

Client selects a local file (e.g., file2.txt)

Client encrypts and sends file

Server decrypts and saves it to server_uploads/

🔐 XOR Encryption Function
void xorEncrypt(char* data, int length, int key = 123) {
    for (int i = 0; i < length; i++) {
        data[i] ^= key;
    }
}


Used in both client and server before sending and after receiving data.
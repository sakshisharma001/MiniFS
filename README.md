# 🚀 MiniFS – Distributed File Storage System

MiniFS is a simple distributed file storage system built in **C**. I created this project to understand how client-server communication, socket programming, multithreading, and file management work together in a real application.
The system allows multiple clients to connect to a server and perform file operations like uploading, downloading, listing, deleting, and viewing file information. To handle multiple users at the same time, the server uses **POSIX threads** and thread synchronization with **mutexes**.

---

## ✨ Features

- Upload and download files between client and server
- List and delete stored files
- Handle multiple client connections simultaneously
- Thread-safe file and metadata management
- Custom binary protocol for communication
- Interactive command-line interface
- Graceful server shutdown

---

## 🛠️ Tech Stack

- C (C11)
- POSIX Sockets
- Pthreads
- Readline
- GCC
- Linux

---

## 📂 Project Structure

```text
client/      Client-side implementation
server/      Server-side implementation
common/      Shared utilities and protocol
storage/     Stored files
docs/        Documentation
```

---

## 🚀 Getting Started

```bash
make

./bin/minifs_server
./bin/minifs_client
```

---

## 📌 Supported Operations

- Upload Files
- Download Files
- List Files
- Delete Files
- View File Information

---

## 💡 What I Learned

While building this project, I got hands-on experience with:

- Socket Programming
- Client-Server Architecture
- Multithreading using Pthreads
- Thread Synchronization with Mutexes
- Binary Data Communication
- File System Operations
- Concurrent Programming

  ---

### ⭐ Thanks for Visiting

If you found this project interesting or helpful, consider giving it a ⭐. Feedback and suggestions are always welcome!

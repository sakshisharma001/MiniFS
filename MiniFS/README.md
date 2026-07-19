# MiniFS - Distributed File Storage System

<div align="center">

![C](https://img.shields.io/badge/C-C11-blue.svg)
![Linux](https://img.shields.io/badge/OS-Linux-orange.svg)
![License](https://img.shields.io/badge/License-Educational-green.svg)
![Threads](https://img.shields.io/badge/Concurrency-Pthreads-red.svg)

**A production-quality distributed file storage system built in C (C11)**

[Quick Start](#quick-start) • [Features](#features) • [Installation](#installation) • [Usage](#usage) • [Demo](#demo)

</div>

---

## 🎯 What is MiniFS?

MiniFS is a **distributed file storage system** that lets you store, retrieve, and manage files through a central server. Think of it like your own personal Google Drive or Dropbox, but built from scratch using C programming language.

**In simple terms:**
- You run a **server** on one machine (like your Linux computer)
- Multiple **clients** can connect to this server from anywhere
- Clients can upload files, download files, list files, delete files, and get file information
- Everything happens **concurrently** - multiple people can use it at the same time without issues

**Why MiniFS?**
- � **Fast**: Uses binary protocol for efficient data transfer
- 🔒 **Safe**: Thread-safe operations prevent data corruption
- 💡 **Educational**: Great for learning systems programming, networking, and concurrency
- �️ **Simple**: Easy to set up and use

---

## ⚡ Quick Start (For Beginners)

If you just want to try it out quickly, follow these steps:

### Step 1: Install Dependencies
```bash
sudo apt-get update
sudo apt-get install build-essential libreadline-dev
```

### Step 2: Build the Project
```bash
cd MiniFS
make
```

### Step 3: Start the Server
Open **Terminal 1**:
```bash
./bin/minifs_server
```
You should see: `MiniFS Server running on port 8080`

### Step 4: Start the Client
Open **Terminal 2**:
```bash
./bin/minifs_client
```
You should see: `MiniFS Client - Connected to 127.0.0.1:8080`

### Step 5: Try Some Commands
```bash
MiniFS> UPLOAD test.txt test.txt
MiniFS> LIST
MiniFS> DOWNLOAD test.txt downloaded.txt
MiniFS> EXIT
```

That's it! You've successfully used MiniFS. 🎉

---

## ✨ Key Features

Here's what makes MiniFS special:

- **� Binary Protocol**: Instead of sending text commands like HTTP, MiniFS uses a custom binary format. This makes data transfer faster and more efficient. Think of it like sending a zip file instead of a text file - much smaller and quicker.

- **⚡ Concurrent Operations**: Multiple clients can use the server at the same time. You can have 10 people uploading files simultaneously without any problems. This is achieved using threads - each client gets its own thread to handle requests.

- **🔒 Thread-Safe**: When multiple people access the same data, things can get messy. MiniFS uses mutexes (mutual exclusions) to ensure that only one thread can modify shared data at a time. This prevents data corruption and race conditions.

- **📊 Metadata Management**: The server keeps track of all files in a metadata table. This includes file names, sizes, upload times, and checksums. It's like a database of all files stored on the server.

- **🛡️ Graceful Shutdown**: When you press Ctrl+C to stop the server, it doesn't just crash. It saves all metadata, closes all connections properly, and cleans up resources. This ensures no data is lost.

- **💻 Interactive Client**: The client has a nice command-line interface with auto-complete (using readline). You can press Tab to complete commands and use arrow keys to navigate through command history.

- **📁 Complete File Operations**: Upload, Download, Delete, List, Get Info, and Exit - everything you need for file management.

- **🎯 Cross-Platform**: Designed for Linux but can be easily ported to other Unix-like systems.

---

## 📁 How the Project is Organized

The project is divided into logical parts. Here's what each folder does:

```
MiniFS/
├── client/              # Client-side code (what users run)
│   ├── client.c         # Main client program - handles user input
│   ├── commands.c       # All the commands (upload, download, etc.)
│   └── commands.h       # Header file for commands
│
├── server/              # Server-side code (the central storage)
│   ├── server.c         # Main server - accepts connections
│   ├── worker.c         # Worker threads - handles individual clients
│   ├── worker.h         # Header for worker
│   ├── storage.c        # Manages actual file storage on disk
│   ├── storage.h        # Header for storage
│   ├── metadata.c       # Keeps track of file information
│   └── metadata.h       # Header for metadata
│
├── common/              # Shared code used by both client and server
│   ├── protocol.h       # Defines how data is sent between client/server
│   ├── protocol.c       # Functions to serialize/deserialize data
│   ├── utils.h          # Helper functions (logging, file operations)
│   └── utils.c          # Implementation of helper functions
│
├── storage/             # Where files are actually stored (created automatically)
├── docs/                # Documentation and diagrams
├── Makefile             # Build instructions (how to compile)
└── README.md            # This file
```

**In simple words:**
- `client/` = The program you run to connectto the server
- `server/` = The program that stores and manages files
- `common/` = Code that both client and server need
- `storage/` = Where your uploaded files are saved

---

## 🛠️ What You Need to Run This

**Operating System:** Linux (Ubuntu, Kali, Debian, etc.)

**Required Software:**
- GCC compiler (for compiling C code)
- pthread library (for multi-threading - comes with most Linux)
- readline library (for nice command-line interface)

**How to install on Ubuntu/Kali:**
```bash
sudo apt-get update
sudo apt-get install build-essential libreadline-dev
```

That's it! Most Linux systems already have pthread.

---

## 📦 How to Install and Build

### Step 1: Get the Code
```bash
# If you cloned from GitHub
cd MiniFS

# Or if you downloaded the zip, extract it first
cd MiniFS
```

### Step 2: Build the Project
```bash
make
```

This will compile everything and create two executables:
- `bin/minifs_server` - The server program
- `bin/minifs_client` - The client program

### Step 3: (Optional) Install System-Wide
```bash
sudo make install
```

This installs the programs to `/usr/local/bin/` so you can run them from anywhere.

### Other Useful Commands
```bash
make clean        # Remove compiled files
make server       # Build only the server
make client       # Build only the client
make uninstall    # Remove system-wide installation
```

---

## 🎮 How to Use MiniFS

### Starting the Server

Open a terminal and run:
```bash
./bin/minifs_server
```

You should see:
```
MiniFS Server running on port 8080
```

**Custom options:**
```bash
# Use different port
./bin/minifs_server -p 9000

# Use different storage directory
./bin/minifs_server -s /path/to/my/storage

# Save logs to a file
./bin/minifs_server -l server.log

# See all options
./bin/minifs_server -h
```

### Starting the Client

Open a NEW terminal and run:
```bash
./bin/minifs_client
```

You should see:
```
MiniFS Client - Connected to 127.0.0.1:8080
Commands: UPLOAD <local> <remote>, DOWNLOAD <remote> <local>, DELETE <file>, LIST, INFO <file>, EXIT
MiniFS>
```

**Custom options:**
```bash
# Connect to different server
./bin/minifs_client -a 192.168.1.100

# Use different port
./bin/minifs_client -p 9000

# See all options
./bin/minifs_client -h
```

### Available Commands

Once connected, you can use these commands:

| Command | What it does | Example |
|---------|--------------|---------|
| `UPLOAD <local> <remote>` | Upload a file from your computer to the server | `UPLOAD /home/me/photo.jpg photo.jpg` |
| `DOWNLOAD <remote> <local>` | Download a file from the server to your computer | `DOWNLOAD photo.jpg /home/me/downloaded.jpg` |
| `DELETE <filename>` | Delete a file from the server | `DELETE photo.jpg` |
| `LIST` | Show all files on the server | `LIST` |
| `INFO <filename>` | Get details about a specific file | `INFO photo.jpg` |
| `EXIT` | Disconnect from the server | `EXIT` |

### Example Session

Here's a complete example of using MiniFS:

```bash
# Terminal 1 - Start server
$ ./bin/minifs_server
MiniFS Server running on port 8080

# Terminal 2 - Start client
$ ./bin/minifs_client
MiniFS Client - Connected to 127.0.0.1:8080
MiniFS> UPLOAD test.txt test.txt
Uploading test.txt (1024 bytes)...
Upload: Success
MiniFS> LIST
test.txt
MiniFS> INFO test.txt
Getting info for test.txt...
Size: 1024
Upload: 1721200000
Modify: 1721200000
Checksum: 1234567890
MiniFS> DOWNLOAD test.txt downloaded.txt
Downloading test.txt...
Download successful (1024 bytes)
MiniFS> EXIT
Exiting...
```

---

## 🧪 Testing the Project

### Basic Manual Test

1. **Start the server** in one terminal
2. **Start the client** in another terminal
3. **Test upload:**
   ```bash
   MiniFS> UPLOAD test.txt test.txt
   ```
4. **Test list:**
   ```bash
   MiniFS> LIST
   ```
5. **Test download:**
   ```bash
   MiniFS> DOWNLOAD test.txt downloaded.txt
   ```
6. **Test info:**
   ```bash
   MiniFS> INFO test.txt
   ```
7. **Test delete:**
   ```bash
   MiniFS> DELETE test.txt
   ```

### Concurrent Testing

Want to test if multiple clients can use it at the same time? There's a test script:

```bash
./tests/test_concurrent.sh
```

This will:
- Start 10 clients at the same time
- Upload different files
- Download files simultaneously
- Test deleting while downloading
- Test duplicate uploads

---

## 🎯 Live Demo - What It Looks Like

### Building the Project

```bash
$ make
gcc -c -o build/common/protocol.o common/protocol.c -I. -pthread
gcc -c -o build/common/utils.o common/utils.c -I. -pthread
gcc -c -o build/server/storage.o server/storage.c -I. -pthread
gcc -c -o build/server/metadata.o server/metadata.c -I. -pthread
gcc -c -o build/server/worker.o server/worker.c -I. -pthread
gcc -c -o build/server/server.o server/server.c -I. -pthread
gcc -c -o build/client/commands.o client/commands.c -I. -pthread
gcc -c -o build/client/client.o client/client.c -I. -pthread
gcc -o bin/minifs_server build/common/protocol.o build/common/utils.o build/server/storage.o build/server/metadata.o build/server/worker.o build/server/server.o -pthread
gcc -o bin/minifs_client build/common/protocol.o build/common/utils.o build/client/commands.o build/client/client.o -pthread -lreadline
Build successful!
```

### Running the Server

```bash
$ ./bin/minifs_server
MiniFS Server running on port 8080
```

### Running the Client

```bash
$ ./bin/minifs_client
MiniFS Client - Connected to 127.0.0.1:8080
Commands: UPLOAD <local> <remote>, DOWNLOAD <remote> <local>, DELETE <file>, LIST, INFO <file>, EXIT
MiniFS> UPLOAD test.txt test.txt
Uploading test.txt (1024 bytes)...
Upload: Success
MiniFS> LIST
test.txt
MiniFS> INFO test.txt
Getting info for test.txt...
Size: 1024
Upload: 1721200000
Modify: 1721200000
Checksum: 1234567890
MiniFS> EXIT
Exiting...
```

---

## ⚙️ Configuration Options

### Server Configuration

| Option | Default | Description |
|--------|---------|-------------|
| `-p <port>` | 8080 | Which port the server listens on |
| `-s <dir>` | `../storage` | Where files are stored |
| `-l <file>` | stdout | Where to save logs (or leave blank for terminal) |

### Client Configuration

| Option | Default | Description |
|--------|---------|-------------|
| `-a <address>` | 127.0.0.1 | Server IP address |
| `-p <port>` | 8080 | Server port |

---

## 🔧 Troubleshooting - Common Problems

**Problem: Server won't start**
- Check if port 8080 is already in use: `netstat -tlnp | grep 8080`
- Make sure you have write permissions for the storage directory
- Try a different port: `./bin/minifs_server -p 9000`

**Problem: Client can't connect**
- Make sure the server is actually running
- Check if you're using the right IP and port
- Check firewall settings

**Problem: Upload fails**
- Make sure the file exists on your computer
- Check if the storage directory has write permissions
- Check if you have enough disk space

**Problem: Weird errors with multiple clients**
- Check if the metadata file is corrupted
- Check system resource limits
- Look at server logs for more details

---

## 🏗️ How It Works - Technical Details

### Server Architecture (For the curious)

The server uses a **multi-threaded** design:

1. **Main Thread** - Listens for new client connections
2. **Worker Threads** - Each client gets its own thread to handle requests
3. **Storage Layer** - Actually saves files to disk
4. **Metadata Layer** - Keeps track of all file information
5. **Protocol Layer** - Handles communication between client and server

### How Concurrency Works

- Each client connection gets its own thread
- Shared data (like metadata) is protected with mutexes
- This prevents two threads from modifying data at the same time
- Multiple clients can read data simultaneously
- Only one client can write at a time

### The Binary Protocol

Instead of sending plain text like HTTP, MiniFS uses binary data:

**Request (Client → Server):**
- 4 bytes: What command (upload, download, etc.)
- 4 bytes: How long the filename is
- 8 bytes: How big the file is
- N bytes: The filename
- N bytes: The actual file data (if uploading)

**Response (Server → Client):**
- 4 bytes: Success or error status
- 4 bytes: Filename length
- 8 bytes: File size
- 4 bytes: Response data size
- N bytes: The filename
- N bytes: The response data (file data if downloading)

This is much more efficient than sending text!

---

## 🔒 Security Notes

**What's secure:**
- Filenames are validated to prevent hacking attempts
- All inputs are sanitized before processing
- Connections timeout after 5 minutes

**What's NOT secure (yet):**
- No username/password authentication
- No encryption (data is sent in plain text)
- No access control (anyone can do anything)

**Future improvements:**
- Add authentication
- Add SSL/TLS encryption
- Add user permissions

---

## 🚀 What's Coming Next

Here are some features planned for the future:

- 🔐 **Authentication** - Username and password login
- 🔒 **Encryption** - SSL/TLS for secure communication
- 📦 **Compression** - Compress files before sending
- ⏸️ **Resumable Downloads** - Continue interrupted downloads
- 🔄 **Replication** - Multiple servers for backup
- 📝 **Config File** - Use a config file instead of command line
- 👤 **User Permissions** - Different users with different access levels
- 💾 **Storage Quotas** - Limit how much each user can store
- 📚 **File Versioning** - Keep multiple versions of files

---

## 🤝 Want to Contribute?

Great! Here's how:

1. **Follow the existing code style** - Keep it consistent
2. **Add comments** - Explain complex functions
3. **Test thoroughly** - Make sure it works before submitting
4. **Update documentation** - If you add features, document them

---

## 📄 License

This project is for educational purposes. Feel free to use it, learn from it, and modify it.

---

## 👨‍💻 About This Project

MiniFS was built to demonstrate:
- Systems programming in C
- Network programming with sockets
- Multi-threading and concurrency
- Binary protocol design
- Thread-safe data structures

It's a great learning project if you want to understand how distributed storage systems work.

---

## 🙏 Acknowled Special Thanks

- POSIX socket programming documentation
- pthread library for making threading easier
- readline library for the nice command-line interface
- All the open-source projects that inspired this

---

<div align="center">

**⭐ If you found this useful, give it a star!**

**Made with ❤️ in C for learning purposes**

</div>

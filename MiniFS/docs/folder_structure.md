# MiniFS Folder Structure Diagram

## Project Directory Structure

```
MiniFS/
│
├── client/                    # Client application
│   ├── client.c              # Main client implementation
│   ├── commands.c            # Client command implementations
│   └── commands.h            # Client command header
│
├── server/                    # Server application
│   ├── server.c              # Main server implementation
│   ├── worker.c              # Worker thread implementation
│   ├── worker.h              # Worker thread header
│   ├── storage.c             # Storage management
│   ├── storage.h             # Storage management header
│   ├── metadata.c            # Metadata management
│   └── metadata.h            # Metadata management header
│
├── common/                    # Shared components
│   ├── protocol.h            # Binary protocol definitions
│   ├── protocol.c            # Protocol serialization
│   ├── utils.h               # Utility functions header
│   └── utils.c               # Utility functions implementation
│
├── storage/                   # File storage directory
│   ├── .metadata             # Metadata file (auto-generated)
│   └── [uploaded files]      # User-uploaded files
│
├── docs/                      # Documentation
│   ├── architecture.md       # Architecture documentation
│   ├── protocol.md           # Protocol documentation
│   └── folder_structure.md   # This file
│
├── build/                     # Build artifacts (generated)
│   ├── client/
│   │   ├── client.o
│   │   └── commands.o
│   ├── server/
│   │   ├── server.o
│   │   ├── worker.o
│   │   ├── storage.o
│   │   └── metadata.o
│   └── common/
│       ├── protocol.o
│       └── utils.o
│
├── bin/                       # Executables (generated)
│   ├── minifs_server         # Server executable
│   └── minifs_client         # Client executable
│
├── tests/                     # Test scripts
│   └── test_concurrent.sh    # Concurrent client test
│
├── Makefile                   # Build configuration
├── README.md                  # Project documentation
└── .gitignore                # Git ignore rules
```

## Directory Purposes

### client/
Contains the client application source code:
- **client.c**: Main client with interactive shell
- **commands.c**: Implementation of client commands
- **commands.h**: Command function declarations

### server/
Contains the server application source code:
- **server.c**: Main server with connection handling
- **worker.c**: Worker thread implementation for client requests
- **worker.h**: Worker thread declarations
- **storage.c**: Physical file storage management
- **storage.h**: Storage function declarations
- **metadata.c**: Metadata table management
- **metadata.h**: Metadata function declarations

### common/
Contains shared components used by both client and server:
- **protocol.h**: Binary protocol structure definitions
- **protocol.c**: Protocol serialization/deserialization
- **utils.h**: Utility function declarations
- **utils.c**: Utility functions (logging, file I/O, etc.)

### storage/
Directory where uploaded files are stored:
- Created automatically by the server
- Contains `.metadata` file for persistence
- Contains user-uploaded files

### docs/
Contains project documentation:
- **architecture.md**: System architecture diagrams
- **protocol.md**: Protocol specification
- **folder_structure.md**: This file

### build/
Generated during compilation:
- Contains object files (.o)
- Organized by source directory structure
- Created by `make` command

### bin/
Generated during compilation:
- Contains final executables
- **minifs_server**: Server executable
- **minifs_client**: Client executable
- Created by `make` command

### tests/
Contains test scripts:
- **test_concurrent.sh**: Tests concurrent client operations

## File Dependencies

### Client Dependencies

```
client.c
├── commands.h
├── commands.c
│   ├── protocol.h
│   ├── protocol.c
│   ├── utils.h
│   └── utils.c
└── readline (system library)
```

### Server Dependencies

```
server.c
├── worker.h
├── worker.c
│   ├── protocol.h
│   ├── protocol.c
│   ├── utils.h
│   ├── utils.c
│   ├── storage.h
│   ├── storage.c
│   ├── metadata.h
│   └── metadata.c
└── pthread (system library)
```

### Common Dependencies

```
protocol.c
├── protocol.h
└── arpa/inet.h (system library)

utils.c
├── utils.h
├── pthread (system library)
└── standard C libraries
```

## Build Output Structure

After running `make`, the following structure is created:

```
MiniFS/
├── build/
│   ├── client/
│   │   ├── client.o
│   │   └── commands.o
│   ├── server/
│   │   ├── server.o
│   │   ├── worker.o
│   │   ├── storage.o
│   │   └── metadata.o
│   └── common/
│       ├── protocol.o
│       └── utils.o
├── bin/
│   ├── minifs_server
│   └── minifs_client
└── storage/
    └── .metadata
```

## Runtime Structure

When the server is running:

```
MiniFS/
├── storage/
│   ├── .metadata              # Metadata persistence file
│   ├── file1.txt              # Uploaded file
│   ├── file2.pdf              # Uploaded file
│   └── file3.jpg              # Uploaded file
└── server.log                # Optional log file (if configured)
```

## Git Ignore Recommendations

The `.gitignore` file should contain:

```
# Build artifacts
build/
bin/

# Storage directory
storage/

# Log files
*.log

# Editor files
*.swp
*.swo
*~
.vscode/
.idea/

# OS files
.DS_Store
Thumbs.db
```

## Installation Structure

After running `sudo make install`:

```
/usr/local/bin/
├── minifs_server
└── minifs_client
```

## Development Workflow

1. **Edit source files** in respective directories
2. **Run `make`** to compile
3. **Run `make run-server`** to test server
4. **Run `make run-client`** to test client
5. **Run `make clean`** to clean build artifacts
6. **Run `make distclean`** to clean everything including storage

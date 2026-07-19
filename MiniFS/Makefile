# Makefile for MiniFS - Distributed File Storage System

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread -g
LDFLAGS = -pthread -lreadline

# Directories
SRC_DIR = .
COMMON_DIR = common
SERVER_DIR = server
CLIENT_DIR = client
BUILD_DIR = build
BIN_DIR = bin

# Server source files
SERVER_SRCS = $(SERVER_DIR)/server.c \
              $(SERVER_DIR)/worker.c \
              $(SERVER_DIR)/storage.c \
              $(SERVER_DIR)/metadata.c \
              $(COMMON_DIR)/protocol.c \
              $(COMMON_DIR)/utils.c

# Client source files
CLIENT_SRCS = $(CLIENT_DIR)/client.c \
              $(CLIENT_DIR)/commands.c \
              $(COMMON_DIR)/protocol.c \
              $(COMMON_DIR)/utils.c

# Object files
SERVER_OBJS = $(SERVER_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
CLIENT_OBJS = $(CLIENT_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Executables
SERVER_EXEC = $(BIN_DIR)/minifs_server
CLIENT_EXEC = $(BIN_DIR)/minifs_client

# Default target
all: directories $(SERVER_EXEC) $(CLIENT_EXEC)

# Create necessary directories
directories:
	@mkdir -p $(BUILD_DIR)/$(SERVER_DIR)
	@mkdir -p $(BUILD_DIR)/$(CLIENT_DIR)
	@mkdir -p $(BUILD_DIR)/$(COMMON_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p storage

# Build server executable
$(SERVER_EXEC): $(SERVER_OBJS)
	@echo "Linking server executable..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Server built successfully: $@"

# Build client executable
$(CLIENT_EXEC): $(CLIENT_OBJS)
	@echo "Linking client executable..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Client built successfully: $@"

# Compile server object files
$(BUILD_DIR)/$(SERVER_DIR)/%.o: $(SERVER_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Compile client object files
$(BUILD_DIR)/$(CLIENT_DIR)/%.o: $(CLIENT_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Compile common object files
$(BUILD_DIR)/$(COMMON_DIR)/%.o: $(COMMON_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)
	@echo "Clean complete"

# Clean everything including storage
distclean: clean
	@echo "Cleaning storage directory..."
	rm -rf storage
	@echo "Distribution clean complete"

# Run server
run-server: $(SERVER_EXEC)
	@echo "Starting server..."
	$(SERVER_EXEC) -p 8080 -s storage

# Run client
run-client: $(CLIENT_EXEC)
	@echo "Starting client..."
	$(CLIENT_EXEC) -a 127.0.0.1 -p 8080

# Install (copy executables to /usr/local/bin)
install: all
	@echo "Installing MiniFS..."
	install -m 755 $(SERVER_EXEC) /usr/local/bin/minifs_server
	install -m 755 $(CLIENT_EXEC) /usr/local/bin/minifs_client
	@echo "Installation complete"

# Uninstall
uninstall:
	@echo "Uninstalling MiniFS..."
	rm -f /usr/local/bin/minifs_server
	rm -f /usr/local/bin/minifs_client
	@echo "Uninstallation complete"

# Help target
help:
	@echo "MiniFS Makefile"
	@echo "================"
	@echo ""
	@echo "Available targets:"
@echo "  all              - Build server and client (default)"
	@echo "clean            - Remove build artifacts"
	@echo "distclean        - Remove build artifacts and storage"
	@echo "run-server       - Build and run server on port 8080"
	@echo "run-client       - Build and run client"
	@echo "install          - Install executables to /usr/local/bin"
	@echo "uninstall        - Remove executables from /usr/local/bin"
	@echo "help             - Show this help message"
	@echo ""
	@echo "Compiler: $(CC)"
	@echo "Flags: $(CFLAGS)"

# Phony targets
.PHONY: all directories clean distclean run-server run-client install uninstall help

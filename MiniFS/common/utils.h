#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

/**
 * @file utils.h
 * @brief Utility functions for MiniFS
 * 
 * This file provides common utility functions used across the project,
 * including logging, file operations, and helper functions.
 */

/* Log levels */
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARN = 2,
    LOG_ERROR = 3
} log_level_t;

/**
 * Initialize the logging system
 * @param log_file Path to log file (NULL for stdout only)
 * @return 0 on success, -1 on error
 */
int utils_init_logging(const char* log_file);

/**
 * Log a message with specified level
 * @param level Log level
 * @param format Printf-style format string
 * @param ... Variable arguments
 */
void utils_log(log_level_t level, const char* format, ...);

/**
 * Close the logging system
 */
void utils_close_logging(void);

/**
 * Validate a filename for security
 * @param filename Filename to validate
 * @return 1 if valid, 0 if invalid
 */
int utils_validate_filename(const char* filename);

/**
 * Sanitize a filename by removing dangerous characters
 * @param filename Filename to sanitize
 * @param output Output buffer
 * @param output_size Size of output buffer
 * @return 0 on success, -1 on error
 */
int utils_sanitize_filename(const char* filename, char* output, size_t output_size);

/**
 * Calculate a simple checksum for data
 * @param data Data buffer
 * @param size Size of data
 * @return Checksum value
 */
uint32_t utils_calculate_checksum(const uint8_t* data, size_t size);

/**
 * Get current timestamp as string
 * @param buffer Output buffer
 * @param buffer_size Size of output buffer
 * @return 0 on success, -1 on error
 */
int utils_get_timestamp(char* buffer, size_t buffer_size);

/**
 * Format file size in human-readable format
 * @param size File size in bytes
 * @param buffer Output buffer
 * @param buffer_size Size of output buffer
 * @return 0 on success, -1 on error
 */
int utils_format_size(uint64_t size, char* buffer, size_t buffer_size);

/**
 * Create a directory if it doesn't exist
 * @param path Directory path
 * @return 0 on success, -1 on error
 */
int utils_create_directory(const char* path);

/**
 * Check if a file exists
 * @param path File path
 * @return 1 if exists, 0 if not
 */
int utils_file_exists(const char* path);

/**
 * Get file size
 * @param path File path
 * @param size Output file size
 * @return 0 on success, -1 on error
 */
int utils_get_file_size(const char* path, uint64_t* size);

/**
 * Read entire file into buffer
 * @param path File path
 * @param buffer Output buffer (caller must free)
 * @param size Output size
 * @return 0 on success, -1 on error
 */
int utils_read_file(const char* path, uint8_t** buffer, size_t* size);

/**
 * Write buffer to file
 * @param path File path
 * @param buffer Data buffer
 * @param size Size of data
 * @return 0 on success, -1 on error
 */
int utils_write_file(const char* path, const uint8_t* buffer, size_t size);

/**
 * Delete a file
 * @param path File path
 * @return 0 on success, -1 on error
 */
int utils_delete_file(const char* path);

/**
 * Set socket to non-blocking mode
 * @param sockfd Socket file descriptor
 * @return 0 on success, -1 on error
 */
int utils_set_nonblocking(int sockfd);

/**
 * Set socket timeout
 * @param sockfd Socket file descriptor
 * @param seconds Timeout in seconds
 * @return 0 on success, -1 on error
 */
int utils_set_socket_timeout(int sockfd, int seconds);

/**
 * Send data over socket with retry logic
 * @param sockfd Socket file descriptor
 * @param data Data buffer
 * @param size Size of data
 * @return Number of bytes sent, or -1 on error
 */
int utils_send_all(int sockfd, const uint8_t* data, size_t size);

/**
 * Receive data from socket with retry logic
 * @param sockfd Socket file descriptor
 * @param buffer Output buffer
 * @param size Size to receive
 * @return Number of bytes received, or -1 on error
 */
int utils_recv_all(int sockfd, uint8_t* buffer, size_t size);

/**
 * Receive exact amount of data from socket
 * @param sockfd Socket file descriptor
 * @param buffer Output buffer
 * @param size Exact size to receive
 * @return 0 on success, -1 on error
 */
int utils_recv_exact(int sockfd, uint8_t* buffer, size_t size);

#endif /* UTILS_H */

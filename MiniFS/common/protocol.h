#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stddef.h>

/**
 * @file protocol.h
 * @brief Binary protocol definitions for MiniFS distributed file storage system
 * 
 * This file defines the custom binary protocol used for client-server communication.
 * The protocol uses a structured binary format for efficient data transfer.
 */

/* Protocol version for compatibility checking */
#define PROTOCOL_VERSION 1

/* Maximum filename length */
#define MAX_FILENAME_LEN 256

/* Maximum buffer size for file transfers */
#define MAX_BUFFER_SIZE 8192

/* Default server port */
#define DEFAULT_PORT 8080

/* Default server address */
#define DEFAULT_SERVER "127.0.0.1"

/**
 * Command types supported by the protocol
 */
typedef enum {
    CMD_UPLOAD = 1,      /* Upload a file to server */
    CMD_DOWNLOAD = 2,    /* Download a file from server */
    CMD_DELETE = 3,      /* Delete a file from server */
    CMD_LIST = 4,        /* List all files on server */
    CMD_INFO = 5,        /* Get information about a specific file */
    CMD_EXIT = 6         /* Exit client session */
} command_type_t;

/**
 * Return status codes for operations
 */
typedef enum {
    STATUS_SUCCESS = 0,          /* Operation completed successfully */
    STATUS_ERROR = 1,            /* General error occurred */
    STATUS_FILE_NOT_FOUND = 2,   /* Requested file does not exist */
    STATUS_FILE_EXISTS = 3,      /* File already exists (for upload) */
    STATUS_PERMISSION_DENIED = 4,/* Permission denied */
    STATUS_INVALID_COMMAND = 5,  /* Invalid command received */
    STATUS_PROTOCOL_ERROR = 6,   /* Protocol parsing error */
    STATUS_SERVER_ERROR = 7      /* Internal server error */
} status_code_t;

/**
 * Request packet structure (client to server)
 * Binary format:
 * [4 bytes] command_type (uint32_t)
 * [4 bytes] filename_length (uint32_t)
 * [4 bytes] file_size (uint64_t)
 * [N bytes] filename (null-terminated string)
 * [N bytes] payload (file data, if applicable)
 */
typedef struct {
    uint32_t command_type;       /* Type of command being sent */
    uint32_t filename_length;    /* Length of filename in bytes */
    uint64_t file_size;          /* Size of file in bytes (0 if no file) */
    char filename[MAX_FILENAME_LEN]; /* Filename (null-terminated) */
    uint8_t* payload;            /* File data pointer (NULL if no file) */
    size_t payload_size;         /* Actual payload size in bytes */
} request_packet_t;

/**
 * Response packet structure (server to client)
 * Binary format:
 * [4 bytes] status (uint32_t)
 * [4 bytes] filename_length (uint32_t)
 * [4 bytes] file_size (uint64_t)
 * [4 bytes] payload_size (uint32_t)
 * [N bytes] filename (null-terminated string)
 * [N bytes] payload (response data, if applicable)
 */
typedef struct {
    uint32_t status;             /* Return status code */
    uint32_t filename_length;    /* Length of filename in bytes */
    uint64_t file_size;          /* Size of file in bytes */
    uint32_t payload_size;       /* Size of payload in bytes */
    char filename[MAX_FILENAME_LEN]; /* Filename (null-terminated) */
    uint8_t* payload;            /* Response data pointer */
} response_packet_t;

/**
 * File metadata structure
 */
typedef struct {
    char filename[MAX_FILENAME_LEN]; /* Filename */
    uint64_t file_size;              /* File size in bytes */
    time_t upload_time;              /* Upload timestamp */
    time_t modify_time;              /* Last modification timestamp */
    uint32_t checksum;              /* Simple checksum for integrity */
} file_metadata_t;

/**
 * Initialize a request packet
 * @param packet Pointer to request packet to initialize
 * @param cmd Command type
 * @param filename Filename (can be NULL for commands without filename)
 * @param file_size Size of file (0 if no file)
 * @param payload File data pointer (NULL if no file)
 * @param payload_size Size of payload (0 if no payload)
 * @return 0 on success, -1 on error
 */
int protocol_init_request(request_packet_t* packet, command_type_t cmd,
                          const char* filename, uint64_t file_size,
                          uint8_t* payload, size_t payload_size);

/**
 * Initialize a response packet
 * @param packet Pointer to response packet to initialize
 * @param status Status code
 * @param filename Filename (can be NULL)
 * @param file_size File size
 * @param payload Response data pointer (NULL if no payload)
 * @param payload_size Size of payload
 * @return 0 on success, -1 on error
 */
int protocol_init_response(response_packet_t* packet, status_code_t status,
                           const char* filename, uint64_t file_size,
                           uint8_t* payload, uint32_t payload_size);

/**
 * Serialize request packet to binary format
 * @param packet Request packet to serialize
 * @param buffer Output buffer (must be large enough)
 * @param buffer_size Size of output buffer
 * @return Number of bytes written, or -1 on error
 */
int protocol_serialize_request(const request_packet_t* packet,
                                uint8_t* buffer, size_t buffer_size);

/**
 * Deserialize request packet from binary format
 * @param buffer Input buffer
 * @param buffer_size Size of input buffer
 * @param packet Output request packet
 * @return 0 on success, -1 on error
 */
int protocol_deserialize_request(const uint8_t* buffer, size_t buffer_size,
                                  request_packet_t* packet);

/**
 * Serialize response packet to binary format
 * @param packet Response packet to serialize
 * @param buffer Output buffer (must be large enough)
 * @param buffer_size Size of output buffer
 * @return Number of bytes written, or -1 on error
 */
int protocol_serialize_response(const response_packet_t* packet,
                                 uint8_t* buffer, size_t buffer_size);

/**
 * Deserialize response packet from binary format
 * @param buffer Input buffer
 * @param buffer_size Size of input buffer
 * @param packet Output response packet
 * @return 0 on success, -1 on error
 */
int protocol_deserialize_response(const uint8_t* buffer, size_t buffer_size,
                                   response_packet_t* packet);

/**
 * Calculate the size of a serialized request packet
 * @param packet Request packet
 * @return Size in bytes, or 0 on error
 */
size_t protocol_request_size(const request_packet_t* packet);

/**
 * Calculate the size of a serialized response packet
 * @param packet Response packet
 * @return Size in bytes, or 0 on error
 */
size_t protocol_response_size(const response_packet_t* packet);

/**
 * Free allocated memory in a packet
 * @param packet Packet to free
 */
void protocol_free_request(request_packet_t* packet);

/**
 * Free allocated memory in a response packet
 * @param packet Response packet to free
 */
void protocol_free_response(response_packet_t* packet);

/**
 * Convert command type to string for logging
 * @param cmd Command type
 * @return String representation
 */
const char* protocol_command_to_string(command_type_t cmd);

/**
 * Convert status code to string for logging
 * @param status Status code
 * @return String representation
 */
const char* protocol_status_to_string(status_code_t status);

#endif /* PROTOCOL_H */

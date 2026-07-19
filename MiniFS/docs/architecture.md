# MiniFS Architecture Documentation

## System Architecture Diagram

```mermaid
graph TB
    subgraph "Client Side"
        C1[Client Process]
        C2[Command Parser]
        C3[Protocol Layer]
        C4[Network Socket]
    end
    
    subgraph "Server Side"
        S1[Main Server Thread]
        S2[Connection Acceptor]
        S3[Worker Thread Pool]
        S4[Metadata Manager]
        S5[Storage Manager]
        S6[Protocol Layer]
        S7[Network Socket]
    end
    
    subgraph "Storage Layer"
        ST1[Physical Storage]
        ST2[Metadata File]
        ST3[File Data]
    end
    
    C1 --> C2
    C2 --> C3
    C3 --> C4
    C4 <--> S7
    S7 --> S6
    S6 --> S3
    S3 --> S4
    S3 --> S5
    S4 --> ST2
    S5 --> ST3
    S1 --> S2
    S2 --> S3
    
    style C1 fill:#e1f5ff
    style S1 fill:#ffe1e1
    style ST1 fill:#e1ffe1
```

## Component Architecture

### Client Components

1. **Main Client Process** (`client.c`)
   - Initializes client context
   - Manages command-line interface
   - Handles user input via readline
   - Coordinates command execution

2. **Command Parser** (`client.c`)
   - Parses user commands
   - Validates command syntax
   - Extracts arguments

3. **Protocol Layer** (`protocol.c`)
   - Serializes requests to binary format
   - Deserializes responses from binary format
   - Handles network byte order conversion

4. **Network Socket**
   - TCP socket connection to server
   - Sends/receives binary data
   - Handles connection errors

### Server Components

1. **Main Server Thread** (`server.c`)
   - Initializes server context
   - Sets up listening socket
   - Handles graceful shutdown
   - Manages worker thread lifecycle

2. **Connection Acceptor** (`server.c`)
   - Accepts incoming client connections
   - Creates worker thread per client
   - Logs connection events

3. **Worker Thread Pool** (`worker.c`)
   - One thread per client connection
   - Processes client requests
   - Executes file operations
   - Sends responses to clients

4. **Metadata Manager** (`metadata.c`)
   - Maintains file metadata table
   - Thread-safe operations with rwlock
   - Persists metadata to disk
   - Validates metadata consistency

5. **Storage Manager** (`storage.c`)
   - Manages physical file storage
   - Handles file I/O operations
   - Thread-safe with mutex
   - Calculates checksums

6. **Protocol Layer** (`protocol.c`)
   - Deserializes client requests
   - Serializes server responses
   - Validates protocol format

## Data Flow

### Upload Flow

```mermaid
sequenceDiagram
    participant C as Client
    participant P as Protocol
    participant W as Worker
    participant M as Metadata
    participant S as Storage
    participant D as Disk
    
    C->>P: Read local file
    P->>P: Create UPLOAD request
    P->>W: Send binary request
    W->>W: Deserialize request
    W->>M: Check if file exists
    M-->>W: File not found
    W->>W: Calculate checksum
    W->>S: Store file data
    S->>D: Write to disk
    D-->>S: Write complete
    S-->>W: Store success
    W->>M: Add metadata entry
    M-->>W: Metadata added
    W->>P: Send SUCCESS response
    P->>C: Display success
```

### Download Flow

```mermaid
sequenceDiagram
    participant C as Client
    participant P as Protocol
    participant W as Worker
    participant M as Metadata
    participant S as Storage
    participant D as Disk
    
    C->>P: Create DOWNLOAD request
    P->>W: Send binary request
    W->>W: Deserialize request
    W->>M: Check if file exists
    M-->>W: File exists
    W->>S: Retrieve file data
    S->>D: Read from disk
    D-->>S: File data
    S-->>W: File data
    W->>P: Send file data in response
    P->>C: Write to local file
    C->>C: Display success
```

### Concurrent Access Flow

```mermaid
sequenceDiagram
    participant C1 as Client 1
    participant C2 as Client 2
    participant W1 as Worker 1
    participant W2 as Worker 2
    participant M as Metadata
    participant S as Storage
    
    C1->>W1: UPLOAD file1.txt
    C2->>W2: LIST files
    
    W1->>M: Acquire write lock
    W2->>M: Acquire read lock
    
    Note over M: Read lock allows<br/>concurrent readers
    
    W1->>S: Store file1.txt
    W2->>M: Get all metadata
    
    W1-->>M: Store complete
    W2-->>M: Metadata retrieved
    
    W1->>M: Release write lock
    W2->>M: Release read lock
    
    W1-->>C1: Upload success
    W2-->>C2: File list
```

## Thread Model

### Server Thread Architecture

```mermaid
graph TB
    subgraph "Main Thread"
        MT1[Initialize Server]
        MT2[Create Socket]
        MT3[Bind & Listen]
        MT4[Accept Loop]
        MT5[Signal Handler]
    end
    
    subgraph "Worker Threads"
        WT1[Worker 1]
        WT2[Worker 2]
        WT3[Worker 3]
        WT4[Worker N]
    end
    
    subgraph "Shared Resources"
        SR1[Metadata Table]
        SR2[Storage Directory]
        SR3[Socket Pool]
    end
    
    MT4 --> WT1
    MT4 --> WT2
    MT4 --> WT3
    MT4 --> WT4
    
    WT1 --> SR1
    WT2 --> SR1
    WT3 --> SR1
    WT4 --> SR1
    
    WT1 --> SR2
    WT2 --> SR2
    WT3 --> SR2
    WT4 --> SR2
    
    MT5 -.-> MT4
    
    style MT1 fill:#ffe1e1
    style SR1 fill:#e1ffe1
```

### Thread Synchronization

- **Metadata Table**: Protected by `pthread_rwlock_t`
  - Read operations: Acquire read lock (multiple readers allowed)
  - Write operations: Acquire write lock (exclusive access)
  
- **Storage Directory**: Protected by `pthread_mutex_t`
  - All file operations require exclusive lock
  
- **Worker Thread Pool**: Protected by `pthread_mutex_t`
  - Worker addition/removal requires lock

## Error Handling

### Error Recovery Flow

```mermaid
graph TD
    A[Operation Start] --> B{Error Occurred?}
    B -->|No| C[Complete Operation]
    B -->|Yes| D{Error Type}
    
    D -->|Network Error| E[Close Connection]
    D -->|File Not Found| F[Send FILE_NOT_FOUND]
    D -->|File Exists| G[Send FILE_EXISTS]
    D -->|Permission Error| H[Send PERMISSION_DENIED]
    D -->|Protocol Error| I[Send PROTOCOL_ERROR]
    D -->|Server Error| J[Send SERVER_ERROR]
    
    E --> K[Log Error]
    F --> K
    G --> K
    H --> K
    I --> K
    J --> K
    
    K --> L[Cleanup Resources]
    L --> M[Continue / Exit]
    
    style A fill:#e1f5ff
    style C fill:#e1ffe1
    style M fill:#ffe1e1
```

## Performance Considerations

### Optimization Strategies

1. **Binary Protocol**: Reduces data transfer overhead
2. **Thread Pool**: Reuses worker threads (future enhancement)
3. **Read-Write Locks**: Allows concurrent metadata reads
4. **Buffered I/O**: Uses efficient buffer sizes
5. **Checksum Calculation**: Simple but effective integrity check

### Scalability Limits

- **Maximum Clients**: 100 concurrent worker threads
- **Maximum Files**: 1000 metadata entries
- **File Size**: Limited by available disk space
- **Filename Length**: 255 characters

## Security Architecture

### Input Validation

```mermaid
graph TD
    A[User Input] --> B{Validate Filename}
    B -->|Invalid| C[Reject Request]
    B -->|Valid| D{Check Path Traversal}
    D -->|Detected| C
    D -->|Clean| E{Sanitize Characters}
    E -->|Invalid| C
    E -->|Valid| F[Process Request]
    
    style A fill:#ffe1e1
    style C fill:#ffcccc
    style F fill:#ccffcc
```

### Security Measures

1. **Filename Validation**: Prevents path traversal attacks
2. **Character Sanitization**: Removes dangerous characters
3. **Timeout Protection**: 5-minute client timeout
4. **Resource Limits**: Maximum file and client limits
5. **Error Handling**: Graceful error responses

## Future Architecture Enhancements

### Planned Improvements

1. **Thread Pool**: Replace thread-per-client with thread pool
2. **Connection Pooling**: Reuse client connections
3. **Caching Layer**: Add in-memory cache for frequently accessed files
4. **Load Balancing**: Distribute load across multiple servers
5. **Replication**: Multi-server data replication
6. **Authentication**: Add user authentication layer
7. **Encryption**: Implement TLS for secure communication

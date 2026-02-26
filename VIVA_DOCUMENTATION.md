# 📚 gRPC File Management Service - Complete Technical Documentation

## For Viva Preparation - Explaining Everything from Scratch

---

# Table of Contents

1. [What is gRPC?](#1-what-is-grpc)
2. [Project Architecture](#2-project-architecture)
3. [Protocol Buffers (Proto File)](#3-protocol-buffers-proto-file)
4. [How gRPC Communication Works](#4-how-grpc-communication-works)
5. [Python Implementation (Working)](#5-python-implementation-working)
6. [C++ Implementation (Reference)](#6-c-implementation-reference)
7. [Python vs C++ Comparison](#7-python-vs-c-comparison)
8. [Step-by-Step Code Explanation](#8-step-by-step-code-explanation)
9. [Common Viva Questions & Answers](#9-common-viva-questions--answers)
10. [How to Run the Project](#10-how-to-run-the-project)

---

# 1. What is gRPC?

## 1.1 Definition

**gRPC** (gRPC Remote Procedure Call) is a modern, high-performance framework developed by Google that allows programs on different computers to communicate with each other.

Think of it like this:
- **Without gRPC**: You write complex code to send HTTP requests, parse JSON, handle errors...
- **With gRPC**: You call a function on the server as if it was a local function!

```
┌─────────────────┐                              ┌─────────────────┐
│  Your Computer  │   "Hey, create a file!"      │  Server         │
│  (Client)       │  ─────────────────────────►  │  (Another PC)   │
│                 │                              │                 │
│  client.        │  ◄─────────────────────────  │  Creates the    │
│  create_file()  │   "Done! File created."      │  file on disk   │
└─────────────────┘                              └─────────────────┘
```

## 1.2 Key Components

| Component | What it does | Analogy |
|-----------|--------------|---------|
| **Protocol Buffers** | Defines the "contract" between client & server | Like a menu in a restaurant |
| **Server** | Listens for requests and processes them | The kitchen that cooks food |
| **Client** | Sends requests to the server | The customer ordering food |
| **Stub** | Auto-generated code that handles network communication | The waiter taking orders |
| **Channel** | The network connection between client and server | The path between table and kitchen |

## 1.3 Why gRPC over REST?

| Feature | REST (Traditional) | gRPC |
|---------|-------------------|------|
| Data Format | JSON (text, slow to parse) | Protocol Buffers (binary, fast) |
| Connection | New connection per request | Persistent HTTP/2 connection |
| Streaming | Not built-in | Built-in support |
| Code Generation | Manual | Automatic |
| Speed | Slower | 10x faster |

---

# 2. Project Architecture

## 2.1 System Overview

```
┌──────────────────────────────────────────────────────────────────────┐
│                        FILE MANAGEMENT SYSTEM                        │
├──────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   ┌─────────────┐         gRPC/HTTP2           ┌─────────────┐      │
│   │             │   Protocol Buffers (Binary)   │             │      │
│   │   CLIENT    │ ════════════════════════════► │   SERVER    │      │
│   │             │ ◄════════════════════════════ │             │      │
│   │  (client.py)│      Port 50051               │ (server.py) │      │
│   │             │                               │             │      │
│   └─────────────┘                               └──────┬──────┘      │
│         │                                              │             │
│         │ User Input                                   │ File I/O    │
│         ▼                                              ▼             │
│   ┌─────────────┐                               ┌─────────────┐      │
│   │   Terminal  │                               │file_storage/│      │
│   │  (Human)    │                               │  (Disk)     │      │
│   └─────────────┘                               └─────────────┘      │
│                                                                      │
└──────────────────────────────────────────────────────────────────────┘
```

## 2.2 File Structure

```
gRPC for Windows/
│
├── proto/                          # SERVICE DEFINITION
│   └── file_service.proto          # The "contract" - defines all operations
│
├── python/                         # PYTHON IMPLEMENTATION ✅ (WORKING)
│   ├── server.py                   # Server code - handles requests
│   ├── client.py                   # Client code - sends requests
│   ├── file_service_pb2.py         # AUTO-GENERATED: Message classes
│   ├── file_service_pb2_grpc.py    # AUTO-GENERATED: Service stubs
│   ├── requirements.txt            # Python dependencies
│   └── file_storage/               # Where files are stored
│       └── (your files here)
│
├── server/                         # C++ IMPLEMENTATION ❌ (NEEDS BUILD TOOLS)
│   ├── file_server.cpp             # C++ server implementation
│   └── file_server.h               # C++ server header
│
├── client/                         # C++ CLIENT
│   ├── file_client.cpp             # C++ client implementation
│   └── file_client.h               # C++ client header
│
├── CMakeLists.txt                  # C++ build configuration
└── vcpkg.json                      # C++ dependency manifest
```

## 2.3 Data Flow (What happens when you create a file)

```
Step 1: User types "create hello.txt Hello World"
                    │
                    ▼
Step 2: Client creates a CreateFileRequest object
        ┌─────────────────────────┐
        │ CreateFileRequest       │
        │   filename: "hello.txt" │
        │   content: "Hello World"│
        └─────────────────────────┘
                    │
                    ▼
Step 3: Request is SERIALIZED (converted to binary)
        [Binary: 0x0A 0x09 0x68 0x65 0x6C 0x6C 0x6F ...]
                    │
                    ▼
Step 4: Sent over network to server (Port 50051)
        ═══════════════════════════════════════►
                    │
                    ▼
Step 5: Server DESERIALIZES (converts binary back to object)
        ┌─────────────────────────┐
        │ CreateFileRequest       │
        │   filename: "hello.txt" │
        │   content: "Hello World"│
        └─────────────────────────┘
                    │
                    ▼
Step 6: Server creates the file on disk
        📁 file_storage/hello.txt
                    │
                    ▼
Step 7: Server creates response
        ┌─────────────────────────┐
        │ CreateFileResponse      │
        │   success: true         │
        │   message: "Created!"   │
        └─────────────────────────┘
                    │
                    ▼
Step 8: Response sent back to client
        ◄═══════════════════════════════════════
                    │
                    ▼
Step 9: Client displays "File created successfully!"
```

---

# 3. Protocol Buffers (Proto File)

## 3.1 What is a Proto File?

The `.proto` file is like a **contract** or **blueprint** that both client and server must follow. It defines:
1. What **services** are available (what operations can be done)
2. What **messages** look like (what data is sent/received)

## 3.2 Our Proto File Explained

**File: `proto/file_service.proto`**

```protobuf
// Line 1: Syntax version - we use proto3 (latest)
syntax = "proto3";

// Line 3: Package name - like a namespace to avoid naming conflicts
package filemanagement;

// Lines 5-15: SERVICE DEFINITION
// This defines WHAT operations the server provides
service FileService {
  // Each 'rpc' is a function that can be called remotely
  
  rpc CreateFile(CreateFileRequest) returns (CreateFileResponse);
  //  ^^^^^^^^^^  ^^^^^^^^^^^^^^^^          ^^^^^^^^^^^^^^^^^^
  //  Function    Input parameter           Return value
  //  name        (what client sends)       (what server returns)
  
  rpc ReadFile(ReadFileRequest) returns (ReadFileResponse);
  rpc WriteFile(WriteFileRequest) returns (WriteFileResponse);
  rpc DeleteFile(DeleteFileRequest) returns (DeleteFileResponse);
  rpc ListFiles(ListFilesRequest) returns (ListFilesResponse);
  rpc CreateDirectory(CreateDirectoryRequest) returns (CreateDirectoryResponse);
  rpc GetFileInfo(GetFileInfoRequest) returns (GetFileInfoResponse);
  
  // STREAMING RPCs - for large files
  rpc UploadFile(stream UploadFileRequest) returns (UploadFileResponse);
  //              ^^^^^^
  //              'stream' means client sends MULTIPLE messages
  
  rpc DownloadFile(DownloadFileRequest) returns (stream DownloadFileResponse);
  //                                             ^^^^^^
  //                                             Server sends MULTIPLE messages
}
```

## 3.3 Message Definitions Explained

```protobuf
// A MESSAGE is like a class/struct - it defines data structure

message CreateFileRequest {
  string filename = 1;    // Field 1: the filename (e.g., "hello.txt")
  string content = 2;     // Field 2: the file content (e.g., "Hello World")
}
//      ^^^^^^    ^
//      Type      Field Number (used in binary encoding, NEVER change!)

message CreateFileResponse {
  bool success = 1;       // Did the operation succeed?
  string message = 2;     // Human-readable message
}

message ReadFileRequest {
  string filename = 1;    // Which file to read
}

message ReadFileResponse {
  bool success = 1;
  string content = 2;     // The file's content
  string message = 3;
}

// For listing files - notice 'repeated' keyword
message ListFilesResponse {
  repeated string files = 1;        // ARRAY of filenames
  repeated string directories = 2;  // ARRAY of directory names
  //^^^^^^
  // 'repeated' means this field can have 0 or more values (like a list)
  
  bool success = 3;
  string message = 4;
}

// For file info
message FileInfo {
  string filename = 1;
  int64 size = 2;           // File size in bytes (64-bit integer)
  int64 modified_time = 3;  // Unix timestamp
  bool is_directory = 4;
  string permissions = 5;
}

// STREAMING message with 'oneof'
message UploadFileRequest {
  oneof data {              // 'oneof' means ONLY ONE of these can be set
    FileMetadata metadata = 1;  // First message: file info
    bytes chunk = 2;            // Subsequent messages: file data chunks
  }
}
// This allows sending metadata first, then file chunks
```

## 3.4 Proto Data Types

| Proto Type | Python Type | C++ Type | Description |
|------------|-------------|----------|-------------|
| `string` | `str` | `std::string` | Text |
| `int32` | `int` | `int32_t` | 32-bit integer |
| `int64` | `int` | `int64_t` | 64-bit integer |
| `bool` | `bool` | `bool` | True/False |
| `bytes` | `bytes` | `std::string` | Binary data |
| `repeated X` | `list[X]` | `std::vector<X>` | Array/List |

---

# 4. How gRPC Communication Works

## 4.1 The Four Types of RPC

```
┌────────────────────────────────────────────────────────────────────────┐
│                        gRPC COMMUNICATION PATTERNS                     │
├────────────────────────────────────────────────────────────────────────┤
│                                                                        │
│  1. UNARY RPC (Simple Request-Response)                               │
│     Client ──── One Request ────► Server                              │
│     Client ◄─── One Response ──── Server                              │
│     Example: CreateFile, ReadFile, DeleteFile                          │
│                                                                        │
│  2. SERVER STREAMING RPC                                               │
│     Client ──── One Request ────► Server                              │
│     Client ◄─── Response 1 ────── Server                              │
│     Client ◄─── Response 2 ────── Server                              │
│     Client ◄─── Response N ────── Server                              │
│     Example: DownloadFile (file sent in chunks)                        │
│                                                                        │
│  3. CLIENT STREAMING RPC                                               │
│     Client ──── Request 1 ──────► Server                              │
│     Client ──── Request 2 ──────► Server                              │
│     Client ──── Request N ──────► Server                              │
│     Client ◄─── One Response ──── Server                              │
│     Example: UploadFile (file sent in chunks)                          │
│                                                                        │
│  4. BIDIRECTIONAL STREAMING RPC                                        │
│     Client ◄───── Messages ─────► Server                              │
│     (Both can send anytime)                                            │
│     Example: Chat application                                          │
│                                                                        │
└────────────────────────────────────────────────────────────────────────┘
```

## 4.2 HTTP/2 Under the Hood

gRPC uses HTTP/2, which provides:

```
HTTP/1.1 (Old):                    HTTP/2 (gRPC uses this):
┌─────────────────┐                ┌─────────────────┐
│ Request 1       │                │ Request 1 ───┐  │
│ Response 1      │                │ Request 2 ──┐│  │
│ Request 2       │                │ Request 3 ─┐││  │
│ Response 2      │                │            │││  │
│ Request 3       │                │ Response 1◄┘││  │
│ Response 3      │                │ Response 3◄─┘│  │
│ (Sequential)    │                │ Response 2◄──┘  │
└─────────────────┘                │ (Parallel!)     │
                                   └─────────────────┘

Multiplexing = Multiple requests/responses at the same time!
```

---

# 5. Python Implementation (Working)

## 5.1 Server Code Explanation (`server.py`)

### Imports Section

```python
"""
gRPC File Service Server - Python Implementation
"""

import grpc                    # Main gRPC library
from concurrent import futures # For thread pool (handling multiple clients)
import os                      # File operations
import time                    # Time operations
import sys                     # System operations

# These are AUTO-GENERATED from the .proto file
import file_service_pb2        # Message classes (CreateFileRequest, etc.)
import file_service_pb2_grpc   # Service classes (FileServiceServicer, etc.)
```

### Storage Configuration

```python
# Where files will be stored on the server
STORAGE_DIR = os.path.join(
    os.path.dirname(os.path.abspath(__file__)),  # Current script directory
    "file_storage"                                # Subfolder name
)
# Result: D:\...\python\file_storage
```

### The Service Implementation Class

```python
class FileServiceServicer(file_service_pb2_grpc.FileServiceServicer):
    """
    This class IMPLEMENTS the FileService defined in the .proto file.
    
    We inherit from FileServiceServicer (auto-generated base class)
    and override each RPC method.
    """
    
    def __init__(self):
        """Constructor - runs when server starts"""
        # Create storage folder if it doesn't exist
        os.makedirs(STORAGE_DIR, exist_ok=True)
        print(f"[Server] Storage directory: {STORAGE_DIR}")
```

### Helper Method - Security

```python
    def _get_full_path(self, filename):
        """
        Converts a filename to a full path.
        
        SECURITY: Uses os.path.basename() to prevent "directory traversal attacks"
        
        Example attack without protection:
            filename = "../../etc/passwd"  # Attacker tries to read system files!
            
        With os.path.basename():
            "../../etc/passwd" → "passwd"  # Only the filename, safe!
        """
        safe_name = os.path.basename(filename)  # Remove any path components
        return os.path.join(STORAGE_DIR, safe_name)
```

### RPC Method: CreateFile

```python
    def CreateFile(self, request, context):
        """
        RPC METHOD: Create a new file
        
        Parameters:
            request: CreateFileRequest object with:
                     - request.filename (string)
                     - request.content (string)
            context: gRPC context (metadata, deadlines, etc.)
        
        Returns:
            CreateFileResponse object with:
                     - success (bool)
                     - message (string)
        """
        try:
            # Step 1: Get safe file path
            filepath = self._get_full_path(request.filename)
            
            # Step 2: Check if file already exists
            if os.path.exists(filepath):
                return file_service_pb2.CreateFileResponse(
                    success=False,
                    message=f"File '{request.filename}' already exists"
                )
            
            # Step 3: Create and write to file
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(request.content)
            
            # Step 4: Return success response
            print(f"[Server] Created file: {request.filename}")
            return file_service_pb2.CreateFileResponse(
                success=True,
                message=f"File '{request.filename}' created successfully"
            )
            
        except Exception as e:
            # Step 5: Handle any errors
            return file_service_pb2.CreateFileResponse(
                success=False,
                message=f"Error creating file: {str(e)}"
            )
```

### RPC Method: ReadFile

```python
    def ReadFile(self, request, context):
        """
        RPC METHOD: Read content from a file
        """
        try:
            filepath = self._get_full_path(request.filename)
            
            # Check if file exists
            if not os.path.exists(filepath):
                return file_service_pb2.ReadFileResponse(
                    success=False,
                    content="",  # Empty content on failure
                    message=f"File '{request.filename}' not found"
                )
            
            # Read file content
            with open(filepath, 'r', encoding='utf-8') as f:
                content = f.read()
            
            return file_service_pb2.ReadFileResponse(
                success=True,
                content=content,  # The actual file content
                message="File read successfully"
            )
            
        except Exception as e:
            return file_service_pb2.ReadFileResponse(
                success=False,
                content="",
                message=f"Error reading file: {str(e)}"
            )
```

### RPC Method: WriteFile (with Append support)

```python
    def WriteFile(self, request, context):
        """
        RPC METHOD: Write or append to a file
        
        request.append = True  → Add to end of file
        request.append = False → Overwrite entire file
        """
        try:
            filepath = self._get_full_path(request.filename)
            
            # Choose file open mode
            mode = 'a' if request.append else 'w'
            # 'a' = append (add to end)
            # 'w' = write (overwrite)
            
            with open(filepath, mode, encoding='utf-8') as f:
                f.write(request.content)
            
            action = "appended to" if request.append else "written to"
            return file_service_pb2.WriteFileResponse(
                success=True,
                message=f"Content {action} '{request.filename}' successfully"
            )
            
        except Exception as e:
            return file_service_pb2.WriteFileResponse(
                success=False,
                message=f"Error writing file: {str(e)}"
            )
```

### RPC Method: ListFiles

```python
    def ListFiles(self, request, context):
        """
        RPC METHOD: List all files and directories
        """
        try:
            files = []
            directories = []
            
            # Iterate through all items in storage directory
            for item in os.listdir(STORAGE_DIR):
                item_path = os.path.join(STORAGE_DIR, item)
                
                if os.path.isfile(item_path):
                    files.append(item)
                elif os.path.isdir(item_path):
                    directories.append(item)
            
            return file_service_pb2.ListFilesResponse(
                files=files,              # List of filenames
                directories=directories,  # List of directory names
                success=True,
                message="Files listed successfully"
            )
            
        except Exception as e:
            return file_service_pb2.ListFilesResponse(
                files=[],
                directories=[],
                success=False,
                message=f"Error listing files: {str(e)}"
            )
```

### RPC Method: UploadFile (Client Streaming)

```python
    def UploadFile(self, request_iterator, context):
        """
        RPC METHOD: Upload a file using STREAMING
        
        Why streaming? For large files!
        - Without streaming: Load entire 1GB file into memory, send at once
        - With streaming: Send 64KB chunks, use minimal memory
        
        Parameters:
            request_iterator: Iterator that yields UploadFileRequest objects
                              First message has metadata, rest have chunks
        """
        try:
            filename = None
            content = b''  # Binary content (bytes)
            
            # Iterate through all incoming messages
            for request in request_iterator:
                # Check what type of data this message contains
                if request.HasField('metadata'):
                    # First message: contains filename and size
                    filename = request.metadata.filename
                    print(f"[Server] Uploading file: {filename}")
                    
                elif request.HasField('chunk'):
                    # Subsequent messages: contain file data chunks
                    content += request.chunk  # Append chunk to content
            
            # All chunks received, save the file
            if filename:
                filepath = self._get_full_path(filename)
                with open(filepath, 'wb') as f:  # 'wb' = write binary
                    f.write(content)
                    
                return file_service_pb2.UploadFileResponse(
                    success=True,
                    message=f"File '{filename}' uploaded ({len(content)} bytes)"
                )
            else:
                return file_service_pb2.UploadFileResponse(
                    success=False,
                    message="No filename provided"
                )
                
        except Exception as e:
            return file_service_pb2.UploadFileResponse(
                success=False,
                message=f"Error uploading file: {str(e)}"
            )
```

### RPC Method: DownloadFile (Server Streaming)

```python
    def DownloadFile(self, request, context):
        """
        RPC METHOD: Download a file using STREAMING
        
        This is a GENERATOR function (uses 'yield' keyword).
        Instead of returning once, it yields multiple responses.
        
        Flow:
            yield metadata → yield chunk1 → yield chunk2 → ... → done
        """
        try:
            filepath = self._get_full_path(request.filename)
            
            if not os.path.exists(filepath):
                return  # No yields = empty stream
            
            file_size = os.path.getsize(filepath)
            
            # FIRST YIELD: Send metadata (filename and size)
            yield file_service_pb2.DownloadFileResponse(
                metadata=file_service_pb2.FileMetadata(
                    filename=request.filename,
                    file_size=file_size
                )
            )
            
            # SUBSEQUENT YIELDS: Send file in 64KB chunks
            CHUNK_SIZE = 64 * 1024  # 64 KB
            
            with open(filepath, 'rb') as f:  # 'rb' = read binary
                while True:
                    chunk = f.read(CHUNK_SIZE)
                    if not chunk:  # Empty = end of file
                        break
                    yield file_service_pb2.DownloadFileResponse(chunk=chunk)
            
            print(f"[Server] Download complete: {request.filename}")
            
        except Exception as e:
            print(f"[Server] Error downloading: {str(e)}")
```

### Server Startup Function

```python
def serve(port=50051):
    """
    Start the gRPC server
    """
    # Step 1: Create server with thread pool
    # ThreadPoolExecutor allows handling 10 clients simultaneously
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    
    # Step 2: Register our service implementation
    file_service_pb2_grpc.add_FileServiceServicer_to_server(
        FileServiceServicer(),  # Our implementation
        server                  # The server
    )
    
    # Step 3: Bind to address
    # '[::]' means listen on all network interfaces
    server_address = f'[::]:{port}'
    server.add_insecure_port(server_address)
    
    # Step 4: Start the server
    server.start()
    
    print("=" * 50)
    print("  gRPC File Service Server (Python)")
    print("=" * 50)
    print(f"  Server listening on port {port}")
    print("=" * 50)
    
    # Step 5: Keep server running
    try:
        server.wait_for_termination()  # Block forever
    except KeyboardInterrupt:
        print("\n[Server] Shutting down...")
        server.stop(0)


if __name__ == '__main__':
    # Allow custom port via command line
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 50051
    serve(port)
```

---

## 5.2 Client Code Explanation (`client.py`)

### The Client Class

```python
class FileServiceClient:
    """
    Client for FileService gRPC service
    
    This class wraps all the gRPC calls in easy-to-use methods.
    """
    
    def __init__(self, host='localhost', port=50051):
        """
        Constructor - establishes connection to server
        
        Parameters:
            host: Server hostname or IP address
            port: Server port number
        """
        # Step 1: Create a CHANNEL (network connection)
        # 'insecure_channel' means no SSL/TLS encryption
        self.channel = grpc.insecure_channel(f'{host}:{port}')
        
        # Step 2: Create a STUB (the "magic" object)
        # The stub has the same methods as FileService in the proto file!
        self.stub = file_service_pb2_grpc.FileServiceStub(self.channel)
        
        print(f"Connected to server at {host}:{port}")
```

### Client Method: create_file

```python
    def create_file(self, filename, content):
        """
        Create a new file on the server
        
        This method:
        1. Creates a request object
        2. Sends it to server via stub
        3. Returns the response
        """
        # Step 1: Create request object
        request = file_service_pb2.CreateFileRequest(
            filename=filename,
            content=content
        )
        
        # Step 2: Call the RPC
        # This looks like a local function call, but actually:
        # - Serializes request to binary
        # - Sends over network
        # - Waits for response
        # - Deserializes response
        # - Returns response object
        response = self.stub.CreateFile(request)
        
        return response
```

### Client Method: upload_file (Streaming)

```python
    def upload_file(self, local_path, remote_filename=None):
        """
        Upload a file using streaming
        
        For large files, we don't want to load everything into memory.
        Instead, we stream it in chunks.
        """
        if not os.path.exists(local_path):
            print(f"Error: Local file '{local_path}' not found")
            return None
        
        if remote_filename is None:
            remote_filename = os.path.basename(local_path)
        
        # This is a GENERATOR FUNCTION
        def generate_chunks():
            """
            Generator that yields upload requests one by one.
            
            Generators are memory-efficient:
            - Instead of creating a list of all chunks
            - We create one chunk at a time
            """
            # FIRST YIELD: Metadata
            file_size = os.path.getsize(local_path)
            yield file_service_pb2.UploadFileRequest(
                metadata=file_service_pb2.FileMetadata(
                    filename=remote_filename,
                    file_size=file_size
                )
            )
            
            # SUBSEQUENT YIELDS: File chunks
            CHUNK_SIZE = 64 * 1024  # 64KB
            with open(local_path, 'rb') as f:
                while True:
                    chunk = f.read(CHUNK_SIZE)
                    if not chunk:
                        break
                    yield file_service_pb2.UploadFileRequest(chunk=chunk)
        
        # Call the streaming RPC
        # The generator is passed directly - gRPC reads from it
        response = self.stub.UploadFile(generate_chunks())
        return response
```

### Interactive Menu

```python
def main():
    """Main interactive loop"""
    # Parse command line arguments
    host = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 50051
    
    # Create client
    client = FileServiceClient(host, port)
    
    # Main loop
    while True:
        print_menu()
        choice = input("Enter choice: ").strip()
        
        if choice == '1':  # Create file
            filename = input("Filename: ").strip()
            content = input("Content: ").strip()
            response = client.create_file(filename, content)
            print(f"Success: {response.success}, Message: {response.message}")
        
        # ... more options ...
        
        elif choice == '0':  # Exit
            print("Goodbye!")
            break
```

---

# 6. C++ Implementation (Reference)

## 6.1 Why C++ Version Doesn't Work

The C++ version requires:

| Requirement | Size | Time to Install |
|-------------|------|-----------------|
| Visual Studio Build Tools | ~3 GB | 10-20 min |
| vcpkg | ~500 MB | 5 min |
| gRPC + dependencies | ~8 GB | 30-60 min |
| **Total** | **~12 GB** | **45-90 min** |

**Error we saw:**
```
error: in triplet x64-windows: Unable to find a valid Visual Studio instance
Could not locate a complete Visual Studio instance
```

This means: Windows C++ builds need Microsoft's compiler (MSVC) which comes with Visual Studio.

## 6.2 C++ Server Code Structure

```cpp
// file_server.cpp

// Include our header and Windows-specific headers
#include "file_server.h"
#include <windows.h>

// Constructor - takes base directory for file storage
FileServiceImpl::FileServiceImpl(const std::string& base_directory) 
    : base_directory_(base_directory) {
    // Create directory if it doesn't exist
    std::filesystem::create_directories(base_directory_);
}

// Security: Get full path safely
std::string FileServiceImpl::GetFullPath(const std::string& filename) {
    std::filesystem::path full_path = 
        std::filesystem::path(base_directory_) / filename;
    return full_path.string();
}

// RPC Method: CreateFile
Status FileServiceImpl::CreateFile(
    ServerContext* context,           // gRPC context
    const CreateFileRequest* request, // Input (pointer)
    CreateFileResponse* response      // Output (pointer)
) {
    try {
        // Validate path
        if (!IsValidPath(request->filename())) {
            response->set_success(false);
            response->set_message("Invalid file path");
            return Status::OK;
        }

        // Get full path and create parent directories
        std::string full_path = GetFullPath(request->filename());
        std::filesystem::create_directories(
            std::filesystem::path(full_path).parent_path()
        );
        
        // Open file for writing
        std::ofstream file(full_path);
        if (!file.is_open()) {
            response->set_success(false);
            response->set_message("Failed to create file");
            return Status::OK;
        }

        // Write content and close
        file << request->content();
        file.close();

        response->set_success(true);
        response->set_message("File created successfully");
        
    } catch (const std::exception& e) {
        response->set_success(false);
        response->set_message("Error: " + std::string(e.what()));
    }
    
    return Status::OK;  // gRPC status (not file operation status)
}
```

## 6.3 C++ Client Code Structure

```cpp
// file_client.cpp

// Constructor - creates stub from channel
FileClient::FileClient(std::shared_ptr<Channel> channel)
    : stub_(FileService::NewStub(channel)) {}

// Method: CreateFile
bool FileClient::CreateFile(
    const std::string& filename, 
    const std::string& content
) {
    // Create request and response objects
    filemanagement::CreateFileRequest request;
    filemanagement::CreateFileResponse response;
    ClientContext context;

    // Set request fields
    request.set_filename(filename);
    request.set_content(content);

    // Call RPC (blocking)
    Status status = stub_->CreateFile(&context, request, &response);

    if (status.ok()) {
        std::cout << "CreateFile: " << response.message() << std::endl;
        return response.success();
    } else {
        std::cout << "CreateFile failed: " << status.error_message() << std::endl;
        return false;
    }
}
```

---

# 7. Python vs C++ Comparison

## 7.1 Side-by-Side Code Comparison

### Creating a File - Server Side

| Python | C++ |
|--------|-----|
| ```python | ```cpp |
| def CreateFile(self, request, context): | Status FileServiceImpl::CreateFile( |
|     filepath = self._get_full_path( |     ServerContext* context, |
|         request.filename) |     const CreateFileRequest* request, |
|     |     CreateFileResponse* response) { |
|     with open(filepath, 'w') as f: |     std::ofstream file(full_path); |
|         f.write(request.content) |     file << request->content(); |
|     | |
|     return file_service_pb2. |     response->set_success(true); |
|         CreateFileResponse( |     response->set_message("OK"); |
|         success=True, |     return Status::OK; |
|         message="Created") | } |
| ``` | ``` |

### Creating a File - Client Side

| Python | C++ |
|--------|-----|
| ```python | ```cpp |
| def create_file(self, filename, content): | bool FileClient::CreateFile( |
|     request = file_service_pb2. |     const std::string& filename, |
|         CreateFileRequest( |     const std::string& content) { |
|         filename=filename, | |
|         content=content |     CreateFileRequest request; |
|     ) |     CreateFileResponse response; |
|     |     ClientContext context; |
|     response = self.stub.CreateFile( | |
|         request) |     request.set_filename(filename); |
|     return response |     request.set_content(content); |
| | |
| |     Status status = stub_->CreateFile( |
| |         &context, request, &response); |
| |     return response.success(); |
| | } |
| ``` | ``` |

## 7.2 Detailed Comparison Table

| Aspect | Python | C++ |
|--------|--------|-----|
| **Lines of Code** | ~300 | ~600 |
| **Setup Time** | 2 minutes | 1+ hours |
| **Dependencies** | 2 packages (grpcio, grpcio-tools) | Visual Studio, vcpkg, CMake, gRPC libs |
| **Disk Space** | ~50 MB | ~12 GB |
| **Build Process** | None (interpreted) | Complex CMake config |
| **Memory Access** | Automatic (garbage collected) | Manual (or smart pointers) |
| **Error Handling** | try/except | try/catch with std::exception |
| **String Handling** | Simple (`str`) | `std::string`, const char*, encoding issues |
| **File I/O** | `open()` with context manager | `std::fstream`, manual close |
| **Type Safety** | Dynamic (runtime errors) | Static (compile-time errors) |
| **Performance** | Good (~10k req/sec) | Excellent (~100k req/sec) |
| **Best For** | Development, prototyping | Production, performance-critical |

## 7.3 Key Syntax Differences

### 1. Variable Declaration

```python
# Python - no type declaration needed
filename = "hello.txt"
content = "Hello World"
success = True
```

```cpp
// C++ - must declare types
std::string filename = "hello.txt";
std::string content = "Hello World";
bool success = true;
```

### 2. Function Parameters

```python
# Python - request is an object
def CreateFile(self, request, context):
    filename = request.filename  # Direct access
```

```cpp
// C++ - request is a pointer
Status CreateFile(ServerContext* context, 
                  const CreateFileRequest* request,
                  CreateFileResponse* response) {
    std::string filename = request->filename();  // Arrow operator
}
```

### 3. Creating Objects

```python
# Python - keyword arguments
response = CreateFileResponse(
    success=True,
    message="OK"
)
```

```cpp
// C++ - setter methods
CreateFileResponse response;
response.set_success(true);
response.set_message("OK");
```

### 4. Error Handling

```python
# Python
try:
    with open(filepath, 'w') as f:
        f.write(content)
except Exception as e:
    print(f"Error: {e}")
```

```cpp
// C++
try {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file");
    }
    file << content;
    file.close();
} catch (const std::exception& e) {
    std::cout << "Error: " << e.what() << std::endl;
}
```

---

# 8. Step-by-Step Code Explanation

## 8.1 What Happens When Server Starts

```
┌─────────────────────────────────────────────────────────────────┐
│                    SERVER STARTUP SEQUENCE                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  Step 1: python server.py                                        │
│          ↓                                                       │
│  Step 2: if __name__ == '__main__':                             │
│              serve(50051)                                        │
│          ↓                                                       │
│  Step 3: server = grpc.server(ThreadPoolExecutor(max_workers=10))│
│          Creates a server with 10 worker threads                 │
│          ↓                                                       │
│  Step 4: add_FileServiceServicer_to_server(FileServiceServicer())│
│          Registers our implementation with the server            │
│          ↓                                                       │
│  Step 5: server.add_insecure_port('[::]:50051')                 │
│          Binds to port 50051 on all network interfaces          │
│          ↓                                                       │
│  Step 6: server.start()                                          │
│          Server begins accepting connections                     │
│          ↓                                                       │
│  Step 7: server.wait_for_termination()                          │
│          Blocks forever, waiting for requests                    │
│                                                                  │
│  [Server is now running and ready!]                              │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## 8.2 What Happens When Client Connects

```
┌─────────────────────────────────────────────────────────────────┐
│                    CLIENT CONNECTION SEQUENCE                    │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  Step 1: python client.py                                        │
│          ↓                                                       │
│  Step 2: client = FileServiceClient('localhost', 50051)          │
│          ↓                                                       │
│  Step 3: channel = grpc.insecure_channel('localhost:50051')      │
│          Creates TCP connection to server                        │
│          ↓                                                       │
│  Step 4: stub = FileServiceStub(channel)                         │
│          Creates the "magic" object with RPC methods             │
│          ↓                                                       │
│  Step 5: [Connection established, ready for RPC calls]           │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

## 8.3 What Happens During an RPC Call

```
┌─────────────────────────────────────────────────────────────────┐
│          COMPLETE RPC FLOW: client.create_file("a.txt", "Hi")   │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  CLIENT SIDE:                                                    │
│  ════════════                                                    │
│  1. Create request object:                                       │
│     request = CreateFileRequest(filename="a.txt", content="Hi")  │
│                                                                  │
│  2. Serialize to binary (Protocol Buffers):                      │
│     Python object → bytes: [0x0A 0x05 0x61 0x2E 0x74 ...]       │
│                                                                  │
│  3. Send HTTP/2 request:                                         │
│     POST /filemanagement.FileService/CreateFile                  │
│     Body: [serialized bytes]                                     │
│                         │                                        │
│                         ▼                                        │
│  ═══════════════ NETWORK ═══════════════                        │
│                         │                                        │
│                         ▼                                        │
│  SERVER SIDE:                                                    │
│  ════════════                                                    │
│  4. Receive HTTP/2 request                                       │
│                                                                  │
│  5. Deserialize bytes back to object:                            │
│     bytes → CreateFileRequest(filename="a.txt", content="Hi")    │
│                                                                  │
│  6. Call our implementation:                                     │
│     FileServiceServicer.CreateFile(request, context)             │
│                                                                  │
│  7. Implementation runs:                                         │
│     - Opens file "file_storage/a.txt"                            │
│     - Writes "Hi"                                                │
│     - Creates response                                           │
│                                                                  │
│  8. Serialize response:                                          │
│     CreateFileResponse(success=True, ...) → bytes                │
│                                                                  │
│  9. Send HTTP/2 response                                         │
│                         │                                        │
│                         ▼                                        │
│  ═══════════════ NETWORK ═══════════════                        │
│                         │                                        │
│                         ▼                                        │
│  CLIENT SIDE:                                                    │
│  ════════════                                                    │
│  10. Receive response bytes                                      │
│                                                                  │
│  11. Deserialize:                                                │
│      bytes → CreateFileResponse(success=True, message="OK")      │
│                                                                  │
│  12. Return to calling code                                      │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

---

# 9. Common Viva Questions & Answers

## Q1: What is gRPC?

**Answer:** gRPC is a high-performance Remote Procedure Call (RPC) framework developed by Google. It allows programs on different computers to communicate by calling functions remotely as if they were local. It uses Protocol Buffers for serialization and HTTP/2 for transport, making it faster than traditional REST APIs.

## Q2: What are Protocol Buffers?

**Answer:** Protocol Buffers (protobuf) is a language-neutral, platform-neutral way of serializing structured data. It's like JSON but binary (smaller and faster). We define data structures in a `.proto` file, and the protobuf compiler generates code for any programming language.

## Q3: How is gRPC different from REST?

**Answer:**
| REST | gRPC |
|------|------|
| Text-based (JSON) | Binary (Protocol Buffers) |
| HTTP/1.1 | HTTP/2 |
| One request per connection | Multiplexed |
| Manual coding | Auto-generated code |
| Slower | 10x faster |

## Q4: What are the types of gRPC communication?

**Answer:**
1. **Unary**: Single request, single response (like REST)
2. **Server Streaming**: Single request, multiple responses
3. **Client Streaming**: Multiple requests, single response
4. **Bidirectional Streaming**: Multiple requests and responses simultaneously

## Q5: Explain the proto file in your project.

**Answer:** The proto file (`file_service.proto`) defines:
1. **Service** - `FileService` with 9 RPC methods (CreateFile, ReadFile, etc.)
2. **Messages** - Data structures like `CreateFileRequest` with fields like `filename` and `content`
3. **Field Numbers** - Each field has a unique number (1, 2, 3...) used in binary encoding

## Q6: What is a Stub in gRPC?

**Answer:** A stub is an auto-generated client-side object that has the same methods as the server service. When you call `stub.CreateFile()`, it:
1. Serializes the request
2. Sends it over the network
3. Waits for response
4. Deserializes and returns the response

The stub hides all networking complexity.

## Q7: What is a Channel in gRPC?

**Answer:** A channel is a connection to a gRPC server. It handles:
- TCP connection management
- TLS/SSL (if secure)
- Connection pooling
- Load balancing (in advanced setups)

We create it with: `grpc.insecure_channel('localhost:50051')`

## Q8: Why does the C++ version need Visual Studio?

**Answer:** The C++ version needs Visual Studio because:
1. gRPC for Windows is built using MSVC (Microsoft Visual C++ Compiler)
2. vcpkg (package manager) builds packages from source using MSVC
3. The CMake build system generates MSVC project files
4. Windows system libraries are linked using MSVC conventions

Without Visual Studio Build Tools, we can't compile C++ code on Windows.

## Q9: How does streaming work in your upload function?

**Answer:** For file uploads:
1. Client sends metadata first (filename, size)
2. Client reads file in 64KB chunks
3. Each chunk is yielded as a separate gRPC message
4. Server receives chunks one by one via iterator
5. Server assembles chunks into complete file
6. When stream ends, server saves file and returns response

This is memory-efficient for large files.

## Q10: What security measures are in your code?

**Answer:**
1. **Path Traversal Prevention**: `os.path.basename()` removes `../` from filenames
2. **Error Handling**: All operations wrapped in try/except
3. **Input Validation**: Checking if files exist before operations
4. **Isolated Storage**: Files stored only in `file_storage/` directory

## Q11: What port does your server use?

**Answer:** Port **50051** (gRPC's conventional default port). It can be changed via command line: `python server.py 8080`

## Q12: What does `[::]` mean in the server address?

**Answer:** `[::]` is IPv6 notation meaning "all interfaces" (like `0.0.0.0` in IPv4). The server listens on:
- `localhost` (127.0.0.1)
- Machine's local IP (e.g., 192.168.1.x)
- Any other network interface

---

# 10. How to Run the Project

## 10.1 Quick Start Commands

```powershell
# Step 1: Navigate to project
cd "D:\project\Group_1_Assignment_1\gRPC for Windows\python"

# Step 2: Install dependencies (one-time)
pip install grpcio grpcio-tools

# Step 3: Generate gRPC code (one-time, or after proto changes)
python -m grpc_tools.protoc -I../proto --python_out=. --grpc_python_out=. ../proto/file_service.proto

# Step 4: Start server (Terminal 1)
python server.py

# Step 5: Start client (Terminal 2)
python client.py
```

## 10.2 Testing the Service

```
==================================================
  gRPC File Service Client
==================================================
  1. Create file
  2. Read file
  3. Write to file
  4. Delete file
  5. List files
  6. Get file info
  7. Upload file
  8. Download file
  9. Create directory
  0. Exit
==================================================
Enter choice: 1
Filename: test.txt
Content: Hello from gRPC!
Success: True, Message: File 'test.txt' created successfully

Enter choice: 2
Filename: test.txt
Content:
Hello from gRPC!

Enter choice: 5
Files: ['test.txt']
Directories: []
```

## 10.3 Verification

Check that files are actually created:
```powershell
ls "D:\project\Group_1_Assignment_1\gRPC for Windows\python\file_storage"

# Output:
# test.txt

cat "D:\project\Group_1_Assignment_1\gRPC for Windows\python\file_storage\test.txt"

# Output:
# Hello from gRPC!
```

---

# Summary

This project demonstrates a **distributed file management system** using **gRPC**. Key takeaways:

1. **gRPC** provides efficient client-server communication using Protocol Buffers
2. **Proto files** define the service contract (what operations exist, what data is sent)
3. **Code generation** creates client stubs and server base classes automatically
4. **Python version** works out-of-the-box with just `pip install`
5. **C++ version** requires complex build setup (Visual Studio, vcpkg, CMake)
6. **Streaming** enables efficient large file transfers
7. **The architecture** separates concerns: proto defines contract, server implements logic, client provides UI

Good luck with your viva! 🎓

# File Management gRPC Project - Setup Guide

## 📋 Project Overview

This is a **gRPC-based File Management System** that provides remote file operations through a client-server architecture. The server exposes file management APIs, and clients can perform operations like create, read, write, delete files and directories.

### Architecture

```
┌─────────────────┐         gRPC/HTTP2          ┌─────────────────┐
│   File Client   │ ◄──────────────────────────►│   File Server   │
│  (Interactive)  │     Protocol Buffers        │ (Port 50051)    │
└─────────────────┘                              └────────┬────────┘
                                                          │
                                                          ▼
                                                 ┌─────────────────┐
                                                 │  ./file_storage │
                                                 │   (Local Dir)   │
                                                 └─────────────────┘
```

### Features

| Operation                             | Description                    |
| ------------------------------------- | ------------------------------ |
| `create <filename> <content>`         | Create a new file with content |
| `read <filename>`                     | Read file contents             |
| `write <filename> <content> [append]` | Write/append to file           |
| `delete <filename>`                   | Delete a file                  |
| `list [directory]`                    | List files and directories     |
| `mkdir <directory>`                   | Create a directory             |
| `info <filename>`                     | Get file metadata              |
| `exit`                                | Exit the client                |

---

## 🔧 Setup Options (Choose One)

### Option A: Using vcpkg (Original - Heavy, ~10GB)

This is the original approach but requires significant disk space and build time.

```powershell
# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat

# Install dependencies (takes 30-60 minutes)
./vcpkg install grpc:x64-windows-static protobuf:x64-windows-static

# Build project
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static
cmake --build build --config Release
```

### Option B: Using Pre-built gRPC Binaries (Recommended - Faster)

Download pre-compiled gRPC libraries to avoid lengthy compilation.

```powershell
# Run the automated setup script
.\setup_project.ps1
```

### Option C: Using Chocolatey (Simplest)

```powershell
# Install Chocolatey if not installed
Set-ExecutionPolicy Bypass -Scope Process -Force
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))

# Install dependencies
choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y
choco install visualstudio2022buildtools -y
choco install grpc -y

# Then build manually
cmake -B build -S .
cmake --build build --config Release
```

---

## 🚀 Quick Start (Recommended Path)

### Prerequisites

1. **Visual Studio 2019/2022** with C++ Desktop Development workload
   - OR Visual Studio Build Tools with C++ components
2. **CMake 3.15+** (can be installed via Visual Studio or separately)
3. **Git** for cloning repositories

### Step 1: Install vcpkg (One-time setup)

```powershell
# Navigate to a permanent location (e.g., C:\tools)
cd C:\tools
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Set environment variable (run as Administrator or add to System Environment Variables)
[Environment]::SetEnvironmentVariable("VCPKG_ROOT", "C:\tools\vcpkg", "User")
$env:VCPKG_ROOT = "C:\tools\vcpkg"
```

### Step 2: Install gRPC Dependencies

```powershell
# This will take 30-60 minutes on first run (cached afterward)
cd $env:VCPKG_ROOT
.\vcpkg install grpc:x64-windows-static protobuf:x64-windows-static
```

### Step 3: Build the Project

```powershell
cd "d:\project\Group_1_Assignment_1\gRPC for Windows"

# Configure with CMake
cmake -B build -S . `
    -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
    -DVCPKG_TARGET_TRIPLET=x64-windows-static `
    -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release
```

### Step 4: Run the Application

**Terminal 1 - Start Server:**

```powershell
cd "d:\project\Group_1_Assignment_1\gRPC for Windows"
.\build\Release\file_server.exe
```

**Terminal 2 - Start Client:**

```powershell
cd "d:\project\Group_1_Assignment_1\gRPC for Windows"
.\build\Release\file_client.exe
```

---

## 📁 Project Structure

```
gRPC for Windows/
├── CMakeLists.txt          # Build configuration
├── vcpkg.json              # Dependency manifest
├── PROJECT_SETUP.md        # This documentation
├── setup_project.ps1       # Automated setup script
│
├── proto/
│   └── file_service.proto  # gRPC service definition
│
├── server/
│   ├── file_server.h       # Server header
│   └── file_server.cpp     # Server implementation
│
├── client/
│   ├── file_client.h       # Client header
│   └── file_client.cpp     # Client implementation
│
├── Release/
│   └── file_storage/       # Default storage directory
│       ├── nik.txt
│       └── surya.txt
│
└── build/                  # Generated build directory
    └── Release/
        ├── file_server.exe
        ├── file_client.exe
        └── *.pb.h/cc       # Generated protobuf files
```

---

## 🔍 Dependency Comparison

| Method              | Disk Space | Build Time | Complexity | Offline Use |
| ------------------- | ---------- | ---------- | ---------- | ----------- |
| **vcpkg (static)**  | ~8-10 GB   | 30-60 min  | Medium     | Yes         |
| **vcpkg (dynamic)** | ~4-5 GB    | 20-40 min  | Medium     | Yes         |
| **Chocolatey**      | ~2-3 GB    | 5-10 min   | Low        | Limited     |
| **Pre-built**       | ~500 MB    | <5 min     | Low        | Yes         |

### Why vcpkg is Still Used

Despite being "heavy", vcpkg provides:

- **Consistent builds** across machines
- **Static linking** (no DLL dependencies at runtime)
- **Version pinning** via `builtin-baseline`
- **CMake integration** out of the box

---

## 🐛 Troubleshooting

### Error: "Cannot find protobuf/gRPC"

Ensure vcpkg toolchain is specified:

```powershell
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="C:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake"
```

### Error: "Runtime library mismatch"

The project uses static runtime (`/MT`). Ensure vcpkg triplet matches:

```powershell
vcpkg install grpc:x64-windows-static
```

### Error: "LNK2038: mismatch detected for '\_ITERATOR_DEBUG_LEVEL'"

Build in Release mode:

```powershell
cmake --build build --config Release
```

### Server starts but client can't connect

1. Check firewall settings
2. Ensure server shows "listening on localhost:50051"
3. Try `127.0.0.1` instead of `localhost`:
   ```powershell
   .\file_client.exe 127.0.0.1
   ```

---

## 📝 Protocol Buffer Definition

The service is defined in `proto/file_service.proto`:

```protobuf
service FileService {
  rpc CreateFile(CreateFileRequest) returns (CreateFileResponse);
  rpc ReadFile(ReadFileRequest) returns (ReadFileResponse);
  rpc WriteFile(WriteFileRequest) returns (WriteFileResponse);
  rpc DeleteFile(DeleteFileRequest) returns (DeleteFileResponse);
  rpc ListFiles(ListFilesRequest) returns (ListFilesResponse);
  rpc CreateDirectory(CreateDirectoryRequest) returns (CreateDirectoryResponse);
  rpc GetFileInfo(GetFileInfoRequest) returns (GetFileInfoResponse);
  rpc UploadFile(stream UploadFileRequest) returns (UploadFileResponse);
  rpc DownloadFile(DownloadFileRequest) returns (stream DownloadFileResponse);
}
```

---

## 📞 Network Configuration

| Parameter      | Default           | Description            |
| -------------- | ----------------- | ---------------------- |
| Server Address | `localhost:50051` | gRPC server endpoint   |
| Base Directory | `./file_storage`  | Where files are stored |

### Custom Configuration

**Server:**

```powershell
.\file_server.exe "0.0.0.0:50051" "D:\my_storage"
```

**Client (connect to remote):**

```powershell
.\file_client.exe "192.168.1.100"
```

---

## 👥 Contributors

- Group 1 Assignment 1

---

## 📄 License

Academic project for educational purposes.

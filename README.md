# gRPC File Management Service

A distributed file management system using gRPC for remote procedure calls.

## 🎯 Quick Start (Python - Recommended)

The **Python version is the simplest** - only 2 dependencies, no complex build system!

### Prerequisites

- Python 3.8+ (you have 3.10 ✓)

### Setup & Run (3 commands!)

```powershell
# 1. Install dependencies
pip install grpcio grpcio-tools

# 2. Generate gRPC code (run from python/ folder)
cd python
python -m grpc_tools.protoc -I../proto --python_out=. --grpc_python_out=. ../proto/file_service.proto

# 3. Run server
python server.py
```

In another terminal:

```powershell
cd python
python client.py
```

---

## 📁 Project Structure

```
gRPC for Windows/
├── proto/
│   └── file_service.proto    # Service definition (shared by all implementations)
├── python/                   # ⭐ Python implementation (SIMPLE!)
│   ├── server.py            # Server implementation
│   ├── client.py            # Interactive client
│   ├── requirements.txt     # Dependencies
│   └── README.md            # Python-specific docs
├── client/                   # C++ client (complex setup)
├── server/                   # C++ server (complex setup)
└── CMakeLists.txt           # C++ build config
```

## ✨ Features

| Operation           | Description                             |
| ------------------- | --------------------------------------- |
| **CreateFile**      | Create a new file with content          |
| **ReadFile**        | Read content from a file                |
| **WriteFile**       | Write/append to a file                  |
| **DeleteFile**      | Delete a file                           |
| **ListFiles**       | List all files and directories          |
| **GetFileInfo**     | Get file metadata (size, modified time) |
| **UploadFile**      | Stream upload a file (for large files)  |
| **DownloadFile**    | Stream download a file                  |
| **CreateDirectory** | Create a new directory                  |

## 🐍 Python vs C++ Comparison

| Aspect            | Python                             | C++                           |
| ----------------- | ---------------------------------- | ----------------------------- |
| **Setup Time**    | 2 minutes                          | 30+ minutes                   |
| **Dependencies**  | `pip install` (2 packages)         | vcpkg + Visual Studio + CMake |
| **Build**         | None needed                        | Complex CMake configuration   |
| **Lines of Code** | ~300                               | ~800                          |
| **Performance**   | Good                               | Better                        |
| **Best For**      | Development, Learning, Prototyping | Production, High-performance  |

## 🔧 Python Setup Details

### Step 1: Install gRPC

```powershell
pip install grpcio grpcio-tools
```

### Step 2: Generate Code

```powershell
cd python
python -m grpc_tools.protoc -I../proto --python_out=. --grpc_python_out=. ../proto/file_service.proto
```

This generates:

- `file_service_pb2.py` - Message classes
- `file_service_pb2_grpc.py` - Service stubs

### Step 3: Run

**Start Server:**

```powershell
python server.py
```

**Start Client (new terminal):**

```powershell
python client.py
```

## 💡 Example Usage

### Interactive Client

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
Filename: hello.txt
Content: Hello World!
Success: True, Message: File 'hello.txt' created successfully
```

### Programmatic Usage

```python
from client import FileServiceClient

# Connect to server
client = FileServiceClient('localhost', 50051)

# Create a file
client.create_file('notes.txt', 'My notes here')

# Read it back
response = client.read_file('notes.txt')
print(response.content)  # "My notes here"

# List all files
response = client.list_files()
print(response.files)  # ['notes.txt', 'hello.txt']

# Upload a local file
client.upload_file('D:/myfile.pdf', 'uploaded.pdf')

# Download a file
client.download_file('uploaded.pdf', 'D:/downloaded.pdf')
```

## 🌐 Network Configuration

**Default:** Server listens on port `50051` on all interfaces

**Connect from another machine:**

```powershell
python client.py <server-ip> 50051
```

**Custom port:**

```powershell
python server.py 8080
python client.py localhost 8080
```

## 📂 File Storage

Files are stored in: `python/file_storage/`

The server automatically creates this directory on startup.

## ❓ Troubleshooting

| Problem                                 | Solution                              |
| --------------------------------------- | ------------------------------------- |
| `ModuleNotFoundError: file_service_pb2` | Run the protoc command (Step 2)       |
| `Connection refused`                    | Make sure server is running           |
| `Port already in use`                   | Change port: `python server.py 50052` |
| `pip not found`                         | Use `python -m pip install ...`       |

## 🏗️ C++ Version (Advanced)

If you need the C++ version for performance reasons:

1. Install [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022)
2. Install vcpkg: `git clone https://github.com/microsoft/vcpkg D:\vcpkg`
3. Bootstrap: `D:\vcpkg\bootstrap-vcpkg.bat`
4. Install gRPC: `vcpkg install grpc:x64-windows` (takes 20-30 min)
5. Build with CMake

**Note:** C++ setup requires ~10GB disk space and Visual Studio Build Tools.

---

## 📝 License

This project is for educational purposes (Group 1 Assignment).

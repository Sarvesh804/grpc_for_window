# gRPC File Service - Python Version

## 🚀 Quick Start (3 Simple Steps!)

### Step 1: Install dependencies

```bash
pip install grpcio grpcio-tools
```

### Step 2: Generate gRPC code

```bash
cd python
python -m grpc_tools.protoc -I../proto --python_out=. --grpc_python_out=. ../proto/file_service.proto
```

### Step 3: Run!

**Terminal 1 - Start Server:**

```bash
python server.py
```

**Terminal 2 - Start Client:**

```bash
python client.py
```

---

## 📁 Project Structure

```
python/
├── server.py           # gRPC server implementation
├── client.py           # Interactive gRPC client
├── requirements.txt    # Python dependencies
├── setup.bat           # Windows setup script
└── file_storage/       # Where files are stored (created automatically)
```

## ✨ Features

| Feature       | Description                             |
| ------------- | --------------------------------------- |
| Create File   | Create a new file with content          |
| Read File     | Read content from existing file         |
| Write File    | Write/append content to file            |
| Delete File   | Remove a file                           |
| List Files    | List all files and directories          |
| Get File Info | Get file metadata (size, modified time) |
| Upload File   | Stream upload a file to server          |
| Download File | Stream download a file from server      |

## 🔧 Why Python over C++?

| Aspect        | C++ (Original)              | Python (This Version) |
| ------------- | --------------------------- | --------------------- |
| Setup Time    | 30+ minutes                 | 2 minutes             |
| Dependencies  | vcpkg, Visual Studio, CMake | Just pip              |
| Build Process | Complex CMake config        | None needed           |
| Code Lines    | ~800                        | ~300                  |
| Performance   | Faster                      | Fast enough           |

## 🛠️ Troubleshooting

**"No module named file_service_pb2"**

- Run the protoc command in Step 2 to generate the code

**"Connection refused"**

- Make sure the server is running first
- Check that port 50051 is not in use

**"pip not found"**

- Use `python -m pip install ...` instead

## 📝 Example Usage

```python
# In your own code, you can use the client like this:
from client import FileServiceClient

client = FileServiceClient('localhost', 50051)

# Create a file
client.create_file('hello.txt', 'Hello World!')

# Read it back
response = client.read_file('hello.txt')
print(response.content)  # "Hello World!"

# List all files
response = client.list_files()
print(response.files)  # ['hello.txt']
```

## 🌐 Network Usage

By default, the server listens on all interfaces (`[::]`):

- Local: `localhost:50051`
- Network: `<your-ip>:50051`

To connect from another machine:

```bash
python client.py <server-ip> 50051
```

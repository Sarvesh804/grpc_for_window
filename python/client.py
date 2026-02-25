"""
gRPC File Service Client - Python Implementation
Interactive client for the file management service
"""

import grpc
import os
import sys

# Add parent directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import file_service_pb2
import file_service_pb2_grpc


class FileServiceClient:
    """Client for FileService gRPC service"""
    
    def __init__(self, host='localhost', port=50051):
        self.channel = grpc.insecure_channel(f'{host}:{port}')
        self.stub = file_service_pb2_grpc.FileServiceStub(self.channel)
        print(f"Connected to server at {host}:{port}")
    
    def create_file(self, filename, content):
        """Create a new file"""
        request = file_service_pb2.CreateFileRequest(
            filename=filename,
            content=content
        )
        response = self.stub.CreateFile(request)
        return response
    
    def read_file(self, filename):
        """Read a file"""
        request = file_service_pb2.ReadFileRequest(filename=filename)
        response = self.stub.ReadFile(request)
        return response
    
    def write_file(self, filename, content, append=False):
        """Write to a file"""
        request = file_service_pb2.WriteFileRequest(
            filename=filename,
            content=content,
            append=append
        )
        response = self.stub.WriteFile(request)
        return response
    
    def delete_file(self, filename):
        """Delete a file"""
        request = file_service_pb2.DeleteFileRequest(filename=filename)
        response = self.stub.DeleteFile(request)
        return response
    
    def list_files(self, directory=""):
        """List files"""
        request = file_service_pb2.ListFilesRequest(directory=directory)
        response = self.stub.ListFiles(request)
        return response
    
    def create_directory(self, directory):
        """Create a directory"""
        request = file_service_pb2.CreateDirectoryRequest(directory=directory)
        response = self.stub.CreateDirectory(request)
        return response
    
    def get_file_info(self, filename):
        """Get file info"""
        request = file_service_pb2.GetFileInfoRequest(filename=filename)
        response = self.stub.GetFileInfo(request)
        return response
    
    def upload_file(self, local_path, remote_filename=None):
        """Upload a file to the server"""
        if not os.path.exists(local_path):
            print(f"Error: Local file '{local_path}' not found")
            return None
        
        if remote_filename is None:
            remote_filename = os.path.basename(local_path)
        
        def generate_chunks():
            # Send metadata first
            file_size = os.path.getsize(local_path)
            yield file_service_pb2.UploadFileRequest(
                metadata=file_service_pb2.FileMetadata(
                    filename=remote_filename,
                    file_size=file_size
                )
            )
            
            # Send file in chunks
            CHUNK_SIZE = 64 * 1024  # 64KB
            with open(local_path, 'rb') as f:
                while True:
                    chunk = f.read(CHUNK_SIZE)
                    if not chunk:
                        break
                    yield file_service_pb2.UploadFileRequest(chunk=chunk)
        
        response = self.stub.UploadFile(generate_chunks())
        return response
    
    def download_file(self, remote_filename, local_path=None):
        """Download a file from the server"""
        if local_path is None:
            local_path = remote_filename
        
        request = file_service_pb2.DownloadFileRequest(filename=remote_filename)
        
        content = b''
        metadata = None
        
        for response in self.stub.DownloadFile(request):
            if response.HasField('metadata'):
                metadata = response.metadata
                print(f"Downloading: {metadata.filename} ({metadata.file_size} bytes)")
            elif response.HasField('chunk'):
                content += response.chunk
        
        if content:
            with open(local_path, 'wb') as f:
                f.write(content)
            return True, f"Downloaded to {local_path}"
        return False, "No content received"


def print_menu():
    """Print interactive menu"""
    print("\n" + "=" * 50)
    print("  gRPC File Service Client")
    print("=" * 50)
    print("  1. Create file")
    print("  2. Read file")
    print("  3. Write to file")
    print("  4. Delete file")
    print("  5. List files")
    print("  6. Get file info")
    print("  7. Upload file")
    print("  8. Download file")
    print("  9. Create directory")
    print("  0. Exit")
    print("=" * 50)


def main():
    """Main interactive loop"""
    host = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 50051
    
    try:
        client = FileServiceClient(host, port)
    except Exception as e:
        print(f"Failed to connect: {e}")
        return
    
    while True:
        print_menu()
        choice = input("Enter choice: ").strip()
        
        try:
            if choice == '1':
                filename = input("Filename: ").strip()
                content = input("Content: ").strip()
                response = client.create_file(filename, content)
                print(f"Success: {response.success}, Message: {response.message}")
            
            elif choice == '2':
                filename = input("Filename: ").strip()
                response = client.read_file(filename)
                if response.success:
                    print(f"Content:\n{response.content}")
                else:
                    print(f"Error: {response.message}")
            
            elif choice == '3':
                filename = input("Filename: ").strip()
                content = input("Content: ").strip()
                append = input("Append? (y/n): ").strip().lower() == 'y'
                response = client.write_file(filename, content, append)
                print(f"Success: {response.success}, Message: {response.message}")
            
            elif choice == '4':
                filename = input("Filename: ").strip()
                response = client.delete_file(filename)
                print(f"Success: {response.success}, Message: {response.message}")
            
            elif choice == '5':
                response = client.list_files()
                if response.success:
                    print("Files:", response.files)
                    print("Directories:", response.directories)
                else:
                    print(f"Error: {response.message}")
            
            elif choice == '6':
                filename = input("Filename: ").strip()
                response = client.get_file_info(filename)
                if response.success:
                    info = response.file_info
                    print(f"  Name: {info.filename}")
                    print(f"  Size: {info.size} bytes")
                    print(f"  Modified: {info.modified_time}")
                    print(f"  Is Directory: {info.is_directory}")
                else:
                    print(f"Error: {response.message}")
            
            elif choice == '7':
                local_path = input("Local file path: ").strip()
                remote_name = input("Remote filename (leave empty for same name): ").strip()
                remote_name = remote_name if remote_name else None
                response = client.upload_file(local_path, remote_name)
                if response:
                    print(f"Success: {response.success}, Message: {response.message}")
            
            elif choice == '8':
                remote_name = input("Remote filename: ").strip()
                local_path = input("Local save path (leave empty for same name): ").strip()
                local_path = local_path if local_path else None
                success, message = client.download_file(remote_name, local_path)
                print(f"Success: {success}, Message: {message}")
            
            elif choice == '9':
                directory = input("Directory name: ").strip()
                response = client.create_directory(directory)
                print(f"Success: {response.success}, Message: {response.message}")
            
            elif choice == '0':
                print("Goodbye!")
                break
            
            else:
                print("Invalid choice. Please try again.")
        
        except grpc.RpcError as e:
            print(f"RPC Error: {e.code()}: {e.details()}")
        except Exception as e:
            print(f"Error: {e}")


if __name__ == '__main__':
    main()

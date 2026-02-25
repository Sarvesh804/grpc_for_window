"""
gRPC File Service Server - Python Implementation
A simple file management server using gRPC
"""

import grpc
from concurrent import futures
import os
import time
import sys

# Add parent directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import file_service_pb2
import file_service_pb2_grpc

# Storage directory
STORAGE_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "file_storage")


class FileServiceServicer(file_service_pb2_grpc.FileServiceServicer):
    """Implementation of FileService gRPC service"""
    
    def __init__(self):
        # Create storage directory if it doesn't exist
        os.makedirs(STORAGE_DIR, exist_ok=True)
        print(f"[Server] Storage directory: {STORAGE_DIR}")
    
    def _get_full_path(self, filename):
        """Get full path for a file, ensuring it's within storage directory"""
        # Prevent directory traversal attacks
        safe_name = os.path.basename(filename)
        return os.path.join(STORAGE_DIR, safe_name)
    
    def CreateFile(self, request, context):
        """Create a new file with content"""
        try:
            filepath = self._get_full_path(request.filename)
            if os.path.exists(filepath):
                return file_service_pb2.CreateFileResponse(
                    success=False,
                    message=f"File '{request.filename}' already exists"
                )
            
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(request.content)
            
            print(f"[Server] Created file: {request.filename}")
            return file_service_pb2.CreateFileResponse(
                success=True,
                message=f"File '{request.filename}' created successfully"
            )
        except Exception as e:
            return file_service_pb2.CreateFileResponse(
                success=False,
                message=f"Error creating file: {str(e)}"
            )
    
    def ReadFile(self, request, context):
        """Read content from a file"""
        try:
            filepath = self._get_full_path(request.filename)
            if not os.path.exists(filepath):
                return file_service_pb2.ReadFileResponse(
                    success=False,
                    content="",
                    message=f"File '{request.filename}' not found"
                )
            
            with open(filepath, 'r', encoding='utf-8') as f:
                content = f.read()
            
            print(f"[Server] Read file: {request.filename}")
            return file_service_pb2.ReadFileResponse(
                success=True,
                content=content,
                message="File read successfully"
            )
        except Exception as e:
            return file_service_pb2.ReadFileResponse(
                success=False,
                content="",
                message=f"Error reading file: {str(e)}"
            )
    
    def WriteFile(self, request, context):
        """Write or append to a file"""
        try:
            filepath = self._get_full_path(request.filename)
            mode = 'a' if request.append else 'w'
            
            with open(filepath, mode, encoding='utf-8') as f:
                f.write(request.content)
            
            action = "appended to" if request.append else "written to"
            print(f"[Server] {action.capitalize()} file: {request.filename}")
            return file_service_pb2.WriteFileResponse(
                success=True,
                message=f"Content {action} '{request.filename}' successfully"
            )
        except Exception as e:
            return file_service_pb2.WriteFileResponse(
                success=False,
                message=f"Error writing file: {str(e)}"
            )
    
    def DeleteFile(self, request, context):
        """Delete a file"""
        try:
            filepath = self._get_full_path(request.filename)
            if not os.path.exists(filepath):
                return file_service_pb2.DeleteFileResponse(
                    success=False,
                    message=f"File '{request.filename}' not found"
                )
            
            os.remove(filepath)
            print(f"[Server] Deleted file: {request.filename}")
            return file_service_pb2.DeleteFileResponse(
                success=True,
                message=f"File '{request.filename}' deleted successfully"
            )
        except Exception as e:
            return file_service_pb2.DeleteFileResponse(
                success=False,
                message=f"Error deleting file: {str(e)}"
            )
    
    def ListFiles(self, request, context):
        """List files in the storage directory"""
        try:
            files = []
            directories = []
            
            for item in os.listdir(STORAGE_DIR):
                item_path = os.path.join(STORAGE_DIR, item)
                if os.path.isfile(item_path):
                    files.append(item)
                elif os.path.isdir(item_path):
                    directories.append(item)
            
            print(f"[Server] Listed files: {len(files)} files, {len(directories)} directories")
            return file_service_pb2.ListFilesResponse(
                files=files,
                directories=directories,
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
    
    def CreateDirectory(self, request, context):
        """Create a directory"""
        try:
            dirpath = os.path.join(STORAGE_DIR, os.path.basename(request.directory))
            os.makedirs(dirpath, exist_ok=True)
            print(f"[Server] Created directory: {request.directory}")
            return file_service_pb2.CreateDirectoryResponse(
                success=True,
                message=f"Directory '{request.directory}' created successfully"
            )
        except Exception as e:
            return file_service_pb2.CreateDirectoryResponse(
                success=False,
                message=f"Error creating directory: {str(e)}"
            )
    
    def GetFileInfo(self, request, context):
        """Get information about a file"""
        try:
            filepath = self._get_full_path(request.filename)
            if not os.path.exists(filepath):
                return file_service_pb2.GetFileInfoResponse(
                    success=False,
                    message=f"File '{request.filename}' not found"
                )
            
            stat = os.stat(filepath)
            file_info = file_service_pb2.FileInfo(
                filename=request.filename,
                size=stat.st_size,
                modified_time=int(stat.st_mtime),
                is_directory=os.path.isdir(filepath),
                permissions=oct(stat.st_mode)[-3:]
            )
            
            print(f"[Server] Got info for: {request.filename}")
            return file_service_pb2.GetFileInfoResponse(
                success=True,
                file_info=file_info,
                message="File info retrieved successfully"
            )
        except Exception as e:
            return file_service_pb2.GetFileInfoResponse(
                success=False,
                message=f"Error getting file info: {str(e)}"
            )
    
    def UploadFile(self, request_iterator, context):
        """Upload a file using streaming"""
        try:
            filename = None
            content = b''
            
            for request in request_iterator:
                if request.HasField('metadata'):
                    filename = request.metadata.filename
                    print(f"[Server] Uploading file: {filename}")
                elif request.HasField('chunk'):
                    content += request.chunk
            
            if filename:
                filepath = self._get_full_path(filename)
                with open(filepath, 'wb') as f:
                    f.write(content)
                print(f"[Server] Upload complete: {filename} ({len(content)} bytes)")
                return file_service_pb2.UploadFileResponse(
                    success=True,
                    message=f"File '{filename}' uploaded successfully ({len(content)} bytes)"
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
    
    def DownloadFile(self, request, context):
        """Download a file using streaming"""
        try:
            filepath = self._get_full_path(request.filename)
            if not os.path.exists(filepath):
                return
            
            file_size = os.path.getsize(filepath)
            
            # Send metadata first
            yield file_service_pb2.DownloadFileResponse(
                metadata=file_service_pb2.FileMetadata(
                    filename=request.filename,
                    file_size=file_size
                )
            )
            
            # Send file in chunks
            CHUNK_SIZE = 64 * 1024  # 64KB chunks
            with open(filepath, 'rb') as f:
                while True:
                    chunk = f.read(CHUNK_SIZE)
                    if not chunk:
                        break
                    yield file_service_pb2.DownloadFileResponse(chunk=chunk)
            
            print(f"[Server] Download complete: {request.filename}")
        except Exception as e:
            print(f"[Server] Error downloading: {str(e)}")


def serve(port=50051):
    """Start the gRPC server"""
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    file_service_pb2_grpc.add_FileServiceServicer_to_server(
        FileServiceServicer(), server
    )
    
    server_address = f'[::]:{port}'
    server.add_insecure_port(server_address)
    server.start()
    
    print("=" * 50)
    print("  gRPC File Service Server (Python)")
    print("=" * 50)
    print(f"  Server listening on port {port}")
    print(f"  Storage: {STORAGE_DIR}")
    print("  Press Ctrl+C to stop")
    print("=" * 50)
    
    try:
        server.wait_for_termination()
    except KeyboardInterrupt:
        print("\n[Server] Shutting down...")
        server.stop(0)


if __name__ == '__main__':
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 50051
    serve(port)

#ifndef FILE_SERVER_H
#define FILE_SERVER_H

#include <grpcpp/grpcpp.h>
#include <grpcpp/server_builder.h>
#include "file_service.grpc.pb.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;

using filemanagement::FileService;
using filemanagement::CreateFileRequest;
using filemanagement::CreateFileResponse;
using filemanagement::ReadFileRequest;
using filemanagement::ReadFileResponse;
using filemanagement::WriteFileRequest;
using filemanagement::WriteFileResponse;
using filemanagement::DeleteFileRequest;
using filemanagement::DeleteFileResponse;
using filemanagement::ListFilesRequest;
using filemanagement::ListFilesResponse;
using filemanagement::CreateDirectoryRequest;
using filemanagement::CreateDirectoryResponse;
using filemanagement::GetFileInfoRequest;
using filemanagement::GetFileInfoResponse;
using filemanagement::UploadFileRequest;
using filemanagement::UploadFileResponse;
using filemanagement::DownloadFileRequest;
using filemanagement::DownloadFileResponse;

class FileServiceImpl final : public FileService::Service {
public:
    FileServiceImpl(const std::string& base_directory);

private:
    Status CreateFile(ServerContext* context, const CreateFileRequest* request,
                     CreateFileResponse* response) override;
    
    Status ReadFile(ServerContext* context, const ReadFileRequest* request,
                   ReadFileResponse* response) override;
    
    Status WriteFile(ServerContext* context, const WriteFileRequest* request,
                    WriteFileResponse* response) override;
    
    Status DeleteFile(ServerContext* context, const DeleteFileRequest* request,
                     DeleteFileResponse* response) override;
    
    Status ListFiles(ServerContext* context, const ListFilesRequest* request,
                    ListFilesResponse* response) override;
    
    Status CreateDirectory(ServerContext* context, const CreateDirectoryRequest* request,
                          CreateDirectoryResponse* response) override;
    
    Status GetFileInfo(ServerContext* context, const GetFileInfoRequest* request,
                      GetFileInfoResponse* response) override;
    
    Status UploadFile(ServerContext* context, ServerReader<UploadFileRequest>* reader,
                     UploadFileResponse* response) override;
    
    Status DownloadFile(ServerContext* context, const DownloadFileRequest* request,
                       ServerWriter<DownloadFileResponse>* writer) override;

    std::string GetFullPath(const std::string& filename);
    bool IsValidPath(const std::string& path);

    std::string base_directory_;
};

void RunServer(const std::string& server_address, const std::string& base_directory);

#endif // FILE_SERVER_H

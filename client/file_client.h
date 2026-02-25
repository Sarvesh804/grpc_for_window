#ifndef FILE_CLIENT_H
#define FILE_CLIENT_H

#include <grpcpp/grpcpp.h>
#include "file_service.grpc.pb.h"
#include <memory>
#include <string>
#include <iostream>
#include <sstream>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using filemanagement::FileService;

class FileClient {
public:
    FileClient(std::shared_ptr<Channel> channel);

    bool CreateFile(const std::string& filename, const std::string& content);
    std::string ReadFile(const std::string& filename);
    bool WriteFile(const std::string& filename, const std::string& content, bool append = false);
    bool DeleteFile(const std::string& filename);
    void ListFiles(const std::string& directory = "");
    bool CreateDirectory(const std::string& directory);
    void GetFileInfo(const std::string& filename);

private:
    std::unique_ptr<FileService::Stub> stub_;
};

// void RunInteractiveClient();
void RunInteractiveClient(FileClient& client);


#endif // FILE_CLIENT_H

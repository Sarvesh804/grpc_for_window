#include "file_client.h"

FileClient::FileClient(std::shared_ptr<Channel> channel)
    : stub_(FileService::NewStub(channel)) {}

bool FileClient::CreateFile(const std::string& filename, const std::string& content) {
    filemanagement::CreateFileRequest request;
    filemanagement::CreateFileResponse response;
    ClientContext context;

    request.set_filename(filename);
    request.set_content(content);

    Status status = stub_->CreateFile(&context, request, &response);

    if (status.ok()) {
        std::cout << "CreateFile: " << response.message() << std::endl;
        return response.success();
    } else {
        std::cout << "CreateFile failed: " << status.error_message() << std::endl;
        return false;
    }
}

std::string FileClient::ReadFile(const std::string& filename) {
    filemanagement::ReadFileRequest request;
    filemanagement::ReadFileResponse response;
    ClientContext context;

    request.set_filename(filename);

    Status status = stub_->ReadFile(&context, request, &response);

    if (status.ok()) {
        std::cout << "ReadFile: " << response.message() << std::endl;
        if (response.success()) {
            return response.content();
        }
    } else {
        std::cout << "ReadFile failed: " << status.error_message() << std::endl;
    }
    return "";
}

bool FileClient::WriteFile(const std::string& filename, const std::string& content, bool append) {
    filemanagement::WriteFileRequest request;
    filemanagement::WriteFileResponse response;
    ClientContext context;

    request.set_filename(filename);
    request.set_content(content);
    request.set_append(append);

    Status status = stub_->WriteFile(&context, request, &response);

    if (status.ok()) {
        std::cout << "WriteFile: " << response.message() << std::endl;
        return response.success();
    } else {
        std::cout << "WriteFile failed: " << status.error_message() << std::endl;
        return false;
    }
}

bool FileClient::DeleteFile(const std::string& filename) {
    filemanagement::DeleteFileRequest request;
    filemanagement::DeleteFileResponse response;
    ClientContext context;

    request.set_filename(filename);

    Status status = stub_->DeleteFile(&context, request, &response);

    if (status.ok()) {
        std::cout << "DeleteFile: " << response.message() << std::endl;
        return response.success();
    } else {
        std::cout << "DeleteFile failed: " << status.error_message() << std::endl;
        return false;
    }
}

void FileClient::ListFiles(const std::string& directory) {
    filemanagement::ListFilesRequest request;
    filemanagement::ListFilesResponse response;
    ClientContext context;

    request.set_directory(directory);

    Status status = stub_->ListFiles(&context, request, &response);

    if (status.ok() && response.success()) {
        std::cout << "\n=== Directory Listing ===" << std::endl;
        std::cout << "Directories:" << std::endl;
        for (const auto& dir : response.directories()) {
            std::cout << "  [DIR]  " << dir << std::endl;
        }
        std::cout << "Files:" << std::endl;
        for (const auto& file : response.files()) {
            std::cout << "  [FILE] " << file << std::endl;
        }
        std::cout << "=========================" << std::endl;
    } else {
        std::cout << "ListFiles failed: " << 
            (status.ok() ? response.message() : status.error_message()) << std::endl;
    }
}

bool FileClient::CreateDirectory(const std::string& directory) {
    filemanagement::CreateDirectoryRequest request;
    filemanagement::CreateDirectoryResponse response;
    ClientContext context;

    request.set_directory(directory);

    Status status = stub_->CreateDirectory(&context, request, &response);

    if (status.ok()) {
        std::cout << "CreateDirectory: " << response.message() << std::endl;
        return response.success();
    } else {
        std::cout << "CreateDirectory failed: " << status.error_message() << std::endl;
        return false;
    }
}

void FileClient::GetFileInfo(const std::string& filename) {
    filemanagement::GetFileInfoRequest request;
    filemanagement::GetFileInfoResponse response;
    ClientContext context;

    request.set_filename(filename);

    Status status = stub_->GetFileInfo(&context, request, &response);

    if (status.ok() && response.success()) {
        const auto& info = response.file_info();
        std::cout << "\n=== File Information ===" << std::endl;
        std::cout << "Filename: " << info.filename() << std::endl;
        std::cout << "Size: " << info.size() << " bytes" << std::endl;
        std::cout << "Modified: " << info.modified_time() << std::endl;
        std::cout << "Type: " << (info.is_directory() ? "Directory" : "File") << std::endl;
        std::cout << "Permissions: " << info.permissions() << std::endl;
        std::cout << "=========================" << std::endl;
    } else {
        std::cout << "GetFileInfo failed: " << 
            (status.ok() ? response.message() : status.error_message()) << std::endl;
    }
}

void RunInteractiveClient(FileClient& client) {
    std::string server_address = "localhost:50051";
    // FileClient client(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));

    std::cout << "=== File Management Client ===" << std::endl;
    std::cout << "Connected to server: " << server_address << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "1. create <filename> <content>" << std::endl;
    std::cout << "2. read <filename>" << std::endl;
    std::cout << "3. write <filename> <content> [append]" << std::endl;
    std::cout << "4. delete <filename>" << std::endl;
    std::cout << "5. list [directory]" << std::endl;
    std::cout << "6. mkdir <directory>" << std::endl;
    std::cout << "7. info <filename>" << std::endl;
    std::cout << "8. exit" << std::endl;
    std::cout << "===============================" << std::endl;

    std::string command;
    while (true) {
        std::cout << "\nEnter command: ";
        std::getline(std::cin, command);

        if (command.empty()) continue;

        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        if (cmd == "exit") {
            break;
        } else if (cmd == "create") {
            std::string filename, content;
            iss >> filename;
            std::getline(iss, content);
            if (!content.empty() && content[0] == ' ') content = content.substr(1);
            client.CreateFile(filename, content);
        } else if (cmd == "read") {
            std::string filename;
            iss >> filename;
            std::string content = client.ReadFile(filename);
            if (!content.empty()) {
                std::cout << "File content:\n" << content << std::endl;
            }
        } else if (cmd == "write") {
            std::string filename, content, append_str;
            iss >> filename;
            std::getline(iss, content);
            if (!content.empty() && content[0] == ' ') content = content.substr(1);
            
            size_t last_space = content.rfind(' ');
            bool append = false;
            if (last_space != std::string::npos) {
                std::string last_word = content.substr(last_space + 1);
                if (last_word == "append") {
                    append = true;
                    content = content.substr(0, last_space);
                }
            }
            client.WriteFile(filename, content, append);
        } else if (cmd == "delete") {
            std::string filename;
            iss >> filename;
            client.DeleteFile(filename);
        } else if (cmd == "list") {
            std::string directory;
            iss >> directory;
            client.ListFiles(directory);
        } else if (cmd == "mkdir") {
            std::string directory;
            iss >> directory;
            client.CreateDirectory(directory);
        } else if (cmd == "info") {
            std::string filename;
            iss >> filename;
            client.GetFileInfo(filename);
        } else {
            std::cout << "Unknown command: " << cmd << std::endl;
        }
    }
}

int main(int argc, char** argv) {
    std::string server_address = "localhost:50051";
    
    // Accept server IP as command line argument
    if (argc > 1) {
        server_address = std::string(argv[1]) + ":50051";
    }
    
    std::cout << "Connecting to " << server_address << std::endl;

    FileClient client(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));

    RunInteractiveClient(client);

    return 0;
}

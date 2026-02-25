#include "file_server.h"
#include <windows.h>
#include <sys/stat.h>
#include <sstream>

FileServiceImpl::FileServiceImpl(const std::string& base_directory) 
    : base_directory_(base_directory) {
    std::filesystem::create_directories(base_directory_);
}

std::string FileServiceImpl::GetFullPath(const std::string& filename) {
    std::filesystem::path full_path = std::filesystem::path(base_directory_) / filename;
    return full_path.string();
}

bool FileServiceImpl::IsValidPath(const std::string& path) {
    std::filesystem::path full_path = GetFullPath(path);
    std::filesystem::path canonical_base = std::filesystem::canonical(base_directory_);
    
    try {
        std::filesystem::path canonical_path = std::filesystem::weakly_canonical(full_path);
        return canonical_path.string().find(canonical_base.string()) == 0;
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

Status FileServiceImpl::CreateFile(ServerContext* context, const CreateFileRequest* request,
                                  CreateFileResponse* response) {
    try {
        if (!IsValidPath(request->filename())) {
            response->set_success(false);
            response->set_message("Invalid file path");
            return Status::OK;
        }

        std::string full_path = GetFullPath(request->filename());
        std::filesystem::create_directories(std::filesystem::path(full_path).parent_path());
        
        std::ofstream file(full_path);
        if (!file.is_open()) {
            response->set_success(false);
            response->set_message("Failed to create file");
            return Status::OK;
        }

        file << request->content();
        file.close();

        response->set_success(true);
        response->set_message("File created successfully");
    } catch (const std::exception& e) {
        response->set_success(false);
        response->set_message("Error: " + std::string(e.what()));
    }
    return Status::OK;
}

Status FileServiceImpl::ReadFile(ServerContext* context, const ReadFileRequest* request,
                                ReadFileResponse* response) {
    try {
        if (!IsValidPath(request->filename())) {
            response->set_success(false);
            response->set_message("Invalid file path");
            return Status::OK;
        }

        std::string full_path = GetFullPath(request->filename());
        
        if (!std::filesystem::exists(full_path)) {
            response->set_success(false);
            response->set_message("File does not exist");
            return Status::OK;
        }

        std::ifstream file(full_path);
        if (!file.is_open()) {
            response->set_success(false);
            response->set_message("Failed to open file");
            return Status::OK;
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();

        response->set_success(true);
        response->set_content(content);
        response->set_message("File read successfully");
    } catch (const std::exception& e) {
        response->set_success(false);
        response->set_message("Error: " + std::string(e.what()));
    }
    return Status::OK;
}

Status FileServiceImpl::WriteFile(ServerContext* context, const WriteFileRequest* request,
                                 WriteFileResponse* response) {
    try {
        if (!IsValidPath(request->filename())) {
            response->set_success(false);
            response->set_message("Invalid file path");
            return Status::OK;
        }

        std::string full_path = GetFullPath(request->filename());
        std::filesystem::create_directories(std::filesystem::path(full_path).parent_path());
        
        std::ios_base::openmode mode = request->append() ? 
            (std::ios::out | std::ios::app) : std::ios::out;
        
        std::ofstream file(full_path, mode);
        if (!file.is_open()) {
            response->set_success(false);
            response->set_message("Failed to open file for writing");
            return Status::OK;
        }

        file << request->content();
        file.close();

        response->set_success(true);
        response->set_message("File written successfully");
    } catch (const std::exception& e) {
        response->set_success(false);
        response->set_message("Error: " + std::string(e.what()));
    }
    return Status::OK;
}

Status FileServiceImpl::DeleteFile(ServerContext* context, const DeleteFileRequest* request,
                                  DeleteFileResponse* response) {
    try {
        if (!IsValidPath(request->filename())) {
            response->set_success(false);
            response->set_message("Invalid file path");
            return Status::OK;
        }

        std::string full_path = GetFullPath(request->filename());
        
        if (!std::filesystem::exists(full_path)) {
            response->set_success(false);
            response->set_message("File does not exist");
            return Status::OK;
        }

        if (std::filesystem::remove(full_path)) {
            response->set_success(true);
            response->set_message("File deleted successfully");
        } else {
            response->set_success(false);
            response->set_message("Failed to delete file");
        }
    } catch (const std::exception& e) {
        response->set_success(false);
        response->set_message("Error: " + std::string(e.what()));
    }
    return Status::OK;
}

Status FileServiceImpl::ListFiles(ServerContext* context, const ListFilesRequest* request,
                                 ListFilesResponse* response) {
    try {
        std::string directory = request->directory().empty() ? "." : request->directory();
        
        if (!IsValidPath(directory)) {
            response->set_success(false);
            response->set_message("Invalid directory path");
            return Status::OK;
        }

        std::string full_path = GetFullPath(directory);
        
        if (!std::filesystem::exists(full_path)) {
            response->set_success(false);
            response->set_message("Directory does not exist");
            return Status::OK;
        }

        for (const auto& entry : std::filesystem::directory_iterator(full_path)) {
            if (entry.is_regular_file()) {
                response->add_files(entry.path().filename().string());
            } else if (entry.is_directory()) {
                response->add_directories(entry.path().filename().string());
            }
        }

        response->set_success(true);
        response->set_message("Directory listed successfully");
    } catch (const std::exception& e) {
        response->set_success(false);
        response->set_message("Error: " + std::string(e.what()));
    }
    return Status::OK;
}

Status FileServiceImpl::CreateDirectory(ServerContext* context, const CreateDirectoryRequest* request,
                                       CreateDirectoryResponse* response) {
    try {
        if (!IsValidPath(request->directory())) {
            response->set_success(false);
            response->set_message("Invalid directory path");
            return Status::OK;
        }

        std::string full_path = GetFullPath(request->directory());
        
        if (std::filesystem::create_directories(full_path)) {
            response->set_success(true);
            response->set_message("Directory created successfully");
        } else {
            response->set_success(false);
            response->set_message("Failed to create directory (might already exist)");
        }
    } catch (const std::exception& e) {
        response->set_success(false);
        response->set_message("Error: " + std::string(e.what()));
    }
    return Status::OK;
}

// Status FileServiceImpl::GetFileInfo(ServerContext* context, const GetFileInfoRequest* request,
//                                    GetFileInfoResponse* response) {
//     try {
//         if (!IsValidPath(request->filename())) {
//             response->set_success(false);
//             response->set_message("Invalid file path");
//             return Status::OK;
//         }

//         std::string full_path = GetFullPath(request->filename());
        
//         if (!std::filesystem::exists(full_path)) {
//             response->set_success(false);
//             response->set_message("File does not exist");
//             return Status::OK;
//         }

//         auto file_info = response->mutable_file_info();
//         file_info->set_filename(request->filename());
        
//         struct stat file_stat;
//         if (stat(full_path.c_str(), &file_stat) == 0) {
//             file_info->set_size(file_stat.st_size);
//             file_info->set_modified_time(file_stat.st_mtime);
//             file_info->set_is_directory(S_ISDIR(file_stat.st_mode));
            
//             std::string permissions = "";
//             if (file_stat.st_mode & S_IREAD) permissions += "r";
//             if (file_stat.st_mode & S_IWRITE) permissions += "w";
//             if (file_stat.st_mode & S_IEXEC) permissions += "x";
//             file_info->set_permissions(permissions);
//         }

//         response->set_success(true);
//         response->set_message("File info retrieved successfully");
//     } catch (const std::exception& e) {
//         response->set_success(false);
//         response->set_message("Error: " + std::string(e.what()));
//     }
//     return Status::OK;
// }
Status FileServiceImpl::GetFileInfo(ServerContext* context, const GetFileInfoRequest* request,
                                   GetFileInfoResponse* response) {
    try {
        if (!IsValidPath(request->filename())) {
            response->set_success(false);
            response->set_message("Invalid file path");
            return Status::OK;
        }

        std::string full_path = GetFullPath(request->filename());
        
        if (!std::filesystem::exists(full_path)) {
            response->set_success(false);
            response->set_message("File does not exist");
            return Status::OK;
        }

        auto file_info = response->mutable_file_info();
        file_info->set_filename(request->filename());
        
        // Use std::filesystem instead of POSIX stat for Windows compatibility
        try {
            auto file_status = std::filesystem::status(full_path);
            file_info->set_is_directory(std::filesystem::is_directory(file_status));
            file_info->set_size(std::filesystem::file_size(full_path));
            
            // Get file modification time
            auto ftime = std::filesystem::last_write_time(full_path);
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - std::filesystem::file_time_type::clock::now() + 
                std::chrono::system_clock::now());
            auto time_t_value = std::chrono::system_clock::to_time_t(sctp);
            file_info->set_modified_time(time_t_value);
            
            // Windows permissions (simplified)
            file_info->set_permissions(std::filesystem::is_directory(file_status) ? "rwx" : "rw");
        } catch (const std::filesystem::filesystem_error&) {
            file_info->set_size(0);
            file_info->set_modified_time(0);
            file_info->set_permissions("unknown");
        }

        response->set_success(true);
        response->set_message("File info retrieved successfully");
    } catch (const std::exception& e) {
        response->set_success(false);
        response->set_message("Error: " + std::string(e.what()));
    }
    return Status::OK;
}


Status FileServiceImpl::UploadFile(ServerContext* context, ServerReader<UploadFileRequest>* reader,
                                  UploadFileResponse* response) {
    response->set_success(true);
    response->set_message("Upload not implemented in basic version");
    return Status::OK;
}

Status FileServiceImpl::DownloadFile(ServerContext* context, const DownloadFileRequest* request,
                                    ServerWriter<DownloadFileResponse>* writer) {
    return Status(grpc::StatusCode::UNIMPLEMENTED, "Download not implemented in basic version");
}

void RunServer(const std::string& server_address, const std::string& base_directory) {
    FileServiceImpl service(base_directory);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "File Management Server listening on " << server_address << std::endl;
    std::cout << "Base directory: " << base_directory << std::endl;

    server->Wait();
}

int main(int argc, char** argv) {
    std::string server_address = "localhost:50051";
    std::string base_directory = "./file_storage";
    
    if (argc > 1) {
        server_address = argv[1];
    }
    if (argc > 2) {
        base_directory = argv[2];
    }

    RunServer(server_address, base_directory);
    return 0;
}

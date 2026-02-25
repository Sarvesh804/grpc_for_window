cmake_minimum_required(VERSION 3.15)
project(FileManagement_gRPC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CRITICAL: Force static runtime to match vcpkg libraries
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Also set compiler flags explicitly
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
endif()

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Proto file generation
set(PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/proto")
set(PROTO_FILE "${PROTO_PATH}/file_service.proto")
set(GENERATED_PROTOBUF_PATH "${CMAKE_CURRENT_BINARY_DIR}")

set(PROTO_SRCS "${GENERATED_PROTOBUF_PATH}/file_service.pb.cc")
set(PROTO_HDRS "${GENERATED_PROTOBUF_PATH}/file_service.pb.h")
set(GRPC_SRCS "${GENERATED_PROTOBUF_PATH}/file_service.grpc.pb.cc")
set(GRPC_HDRS "${GENERATED_PROTOBUF_PATH}/file_service.grpc.pb.h")

get_target_property(PROTOC_EXECUTABLE protobuf::protoc IMPORTED_LOCATION_RELEASE)
get_target_property(GRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin IMPORTED_LOCATION_RELEASE)

add_custom_command(
    OUTPUT ${PROTO_SRCS} ${PROTO_HDRS}
    COMMAND ${PROTOC_EXECUTABLE}
    ARGS --cpp_out=${GENERATED_PROTOBUF_PATH}
         --proto_path=${PROTO_PATH}
         ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
    COMMENT "Generating protobuf files"
)

add_custom_command(
    OUTPUT ${GRPC_SRCS} ${GRPC_HDRS}
    COMMAND ${PROTOC_EXECUTABLE}
    ARGS --grpc_out=${GENERATED_PROTOBUF_PATH}
         --cpp_out=${GENERATED_PROTOBUF_PATH}
         --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXECUTABLE}
         --proto_path=${PROTO_PATH}
         ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
    COMMENT "Generating gRPC files"
)

add_library(file_service_proto
    ${PROTO_SRCS}
    ${GRPC_SRCS}
)

target_link_libraries(file_service_proto
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)

target_include_directories(file_service_proto
    PUBLIC
        ${GENERATED_PROTOBUF_PATH}
)

# Server executable
add_executable(file_server server/file_server.cpp)
target_link_libraries(file_server 
    PRIVATE 
        file_service_proto
        protobuf::libprotobuf
        gRPC::grpc++
)

# Client executable  
add_executable(file_client client/file_client.cpp)
target_link_libraries(file_client 
    PRIVATE 
        file_service_proto
        protobuf::libprotobuf
        gRPC::grpc++
)

# cmake_minimum_required(VERSION 3.15)
# project(FileManagement_gRPC)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(protobuf CONFIG REQUIRED)
# find_package(gRPC CONFIG REQUIRED)

# # Define proto files
# set(PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/proto")
# set(PROTO_FILE "${PROTO_PATH}/file_service.proto")
# set(GENERATED_PROTOBUF_PATH "${CMAKE_CURRENT_BINARY_DIR}")

# # Generated file names
# set(PROTO_SRCS "${GENERATED_PROTOBUF_PATH}/file_service.pb.cc")
# set(PROTO_HDRS "${GENERATED_PROTOBUF_PATH}/file_service.pb.h")
# set(GRPC_SRCS "${GENERATED_PROTOBUF_PATH}/file_service.grpc.pb.cc")
# set(GRPC_HDRS "${GENERATED_PROTOBUF_PATH}/file_service.grpc.pb.h")

# # Get protoc and grpc_cpp_plugin executables
# get_target_property(PROTOC_EXECUTABLE protobuf::protoc IMPORTED_LOCATION_RELEASE)
# get_target_property(GRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin IMPORTED_LOCATION_RELEASE)

# # Generate .pb.cc and .pb.h
# add_custom_command(
#     OUTPUT ${PROTO_SRCS} ${PROTO_HDRS}
#     COMMAND ${PROTOC_EXECUTABLE}
#     ARGS --cpp_out=${GENERATED_PROTOBUF_PATH}
#          --proto_path=${PROTO_PATH}
#          ${PROTO_FILE}
#     DEPENDS ${PROTO_FILE}
#     COMMENT "Generating protobuf files"
# )

# # Generate .grpc.pb.cc and .grpc.pb.h
# add_custom_command(
#     OUTPUT ${GRPC_SRCS} ${GRPC_HDRS}
#     COMMAND ${PROTOC_EXECUTABLE}
#     ARGS --grpc_out=${GENERATED_PROTOBUF_PATH}
#          --cpp_out=${GENERATED_PROTOBUF_PATH}
#          --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXECUTABLE}
#          --proto_path=${PROTO_PATH}
#          ${PROTO_FILE}
#     DEPENDS ${PROTO_FILE}
#     COMMENT "Generating gRPC files"
# )

# # Create proto library
# add_library(file_service_proto
#     ${PROTO_SRCS}
#     ${GRPC_SRCS}
# )

# target_link_libraries(file_service_proto
#     PUBLIC
#         protobuf::libprotobuf
#         gRPC::grpc
#         gRPC::grpc++
# )

# target_include_directories(file_service_proto
#     PUBLIC
#         ${GENERATED_PROTOBUF_PATH}
# )

# # Server executable
# add_executable(file_server server/file_server.cpp)
# target_link_libraries(file_server 
#     PRIVATE 
#         file_service_proto
#         protobuf::libprotobuf
#         gRPC::grpc++
# )
# target_include_directories(file_server 
#     PRIVATE 
#         ${GENERATED_PROTOBUF_PATH}
# )

# # Client executable
# add_executable(file_client client/file_client.cpp)
# target_link_libraries(file_client 
#     PRIVATE 
#         file_service_proto
#         protobuf::libprotobuf
#         gRPC::grpc++
# )
# target_include_directories(file_client 
#     PRIVATE 
#         ${GENERATED_PROTOBUF_PATH}
# )

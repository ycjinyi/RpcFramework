set(SRC_LIST calluserservice.cc ../user.pb.cc)
add_executable(user ${SRC_LIST})
target_link_libraries(user mprpc protobuf)
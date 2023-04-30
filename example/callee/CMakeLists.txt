#添加源文件列表
set(SRC_LIST userservice.cc ../user.pb.cc)
#添加可执行文件列表
add_executable(provider ${SRC_LIST})
#设置动态链接库
target_link_libraries(provider mprpc protobuf)
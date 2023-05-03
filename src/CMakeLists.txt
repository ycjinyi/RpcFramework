#把当前路径.下的所有文件放入SRC_LIST
#aux_source_directory(. SRC_LIST)
set(SRC_LIST mprpcapplication.cc mprpcconfig.cc rpcheader.pb.cc 
    rpcprovider.cc rpcchannel.cc mprpccontroller.cc)
#输出动态库
#add_library(mprpc SHARED ${SRC_LIST})
#输出静态库
add_library(mprpc ${SRC_LIST})
#添加链接需要的静态库，由于muduo库是静态的，因此生成静态库是可以的
target_link_libraries(mprpc muduo_net muduo_base pthread)

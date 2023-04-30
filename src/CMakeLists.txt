#把当前路径.下的所有文件放入SRC_LIST
aux_source_directory(. SRC_LIST)
#输出动态库
add_library(mprpc SHARED ${SRC_LIST})
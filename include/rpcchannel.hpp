#pragma once
#include "user.pb.h"
#include <google/protobuf/service.h>

class MpRpcChannel: public google::protobuf::RpcChannel {
public:
    virtual void CallMethod(const google::protobuf::MethodDescriptor* method,
        google::protobuf::RpcController* controller, 
        const google::protobuf::Message* request,
        google::protobuf::Message* response, 
        google::protobuf::Closure* done) override;
};
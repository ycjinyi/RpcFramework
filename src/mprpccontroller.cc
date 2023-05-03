#include "mprpccontroller.hpp"

MpRpcController::MpRpcController(): failed(false), msg("") {}

MpRpcController::~MpRpcController() {}

void MpRpcController::Reset() { failed = false, msg = ""; }

bool MpRpcController::Failed() const { return failed; }

std::string MpRpcController::ErrorText() const { return msg; }

void MpRpcController::SetFailed(const std::string& reason) { 
    failed = true, msg = reason;}

void MpRpcController::StartCancel() {}

bool MpRpcController::IsCanceled() const { return false;}

void MpRpcController::NotifyOnCancel(google::protobuf::Closure* callback) {}


#include <google/protobuf/service.h>
#include <string>

class MpRpcController: public google::protobuf::RpcController {
public:
    MpRpcController();
    virtual  ~MpRpcController() override;
    virtual void Reset() override;
    virtual bool Failed() const override;
    virtual std::string ErrorText() const override;
    virtual void SetFailed(const std::string& reason) override;
    //空实现
    virtual void StartCancel() override;
    virtual bool IsCanceled() const override;
    virtual void NotifyOnCancel(google::protobuf::Closure* callback) override;
private:
    bool failed;
    std::string msg;
};

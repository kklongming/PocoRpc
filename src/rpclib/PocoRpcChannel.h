/* 
 * File:   PocoRpcChannel.h
 * Author: kk
 *
 * Created on 2013年8月12日, 下午7:33
 */

#ifndef POCORPCCHANNEL_H
#define	POCORPCCHANNEL_H

#include <google/protobuf/service.h>
#include <Poco/AutoPtr.h>

#include "base/base.h"
#include "base/fifo_queue.h"

namespace Poco {

class Thread;
class FastMutex;
class Runnable;

namespace Net {

class SocketAddress;
class StreamSocket;
class SocketReactor;
class ReadableNotification;
class WritableNotification;
class ErrorNotification;
class ShutdownNotification;

}
}

namespace PocoRpc {

class PocoRpcController;
class BytesBuffer;

typedef Poco::AutoPtr<PocoRpcController> AutoPocoRpcControllerPtr;

class PocoRpcChannel : public google::protobuf::RpcChannel {
 public:
  friend class PocoRpcController;
  
  PocoRpcChannel(const std::string& host, uint16 port);
  virtual ~PocoRpcChannel();
  
  void init();

  virtual void CallMethod(const google::protobuf::MethodDescriptor* method,
          google::protobuf::RpcController* controller,
          const google::protobuf::Message* request,
          google::protobuf::Message* response,
          google::protobuf::Closure* done);

  bool Connect();
  void Exit();
  void set_auto_reconnect(bool auto_reconnect);
  uint32 get_re_connect_times();
  
  void NotifyOnReConnectFaild(google::protobuf::Closure* callback);

  const std::string& get_uuid();
  std::string DebugString();

  AutoPocoRpcControllerPtr NewRpcController();
  
 private:
  
  typedef std::map<uint64, AutoPocoRpcControllerPtr> PocoRpcControllerMap;
  typedef FifoQueue<BytesBuffer*> BytesBufferQueue;
  typedef FifoQueue<AutoPocoRpcControllerPtr> RpcControllerQueue;

  std::string uuid_;
  
  // 指示是否终止
  bool exit_;

  // 标记是否已经连接上rpc server
  bool connected_;
  
  // 尝试重连的次数
  uint32 re_connect_times_;
  bool auto_reconnect_;

  // 等待处理的Rpc队列
  scoped_ptr<RpcControllerQueue> rpc_pending_;

  // 已发送Request, 等待Response的RPC map
  scoped_ptr<PocoRpcControllerMap> rpc_waiting_;
  scoped_ptr<Poco::FastMutex> mutex_waiting_response_;

  // 正在发送中的rpc
  AutoPocoRpcControllerPtr rpc_sending_;

  // 正在发送中的rpc的request对应的buf
  scoped_ptr<BytesBuffer> buf_sending_;

  // 正在接受中的rpc的response对应的buf
  scoped_ptr<BytesBuffer> buf_recving_;

  // 已经接收完毕的rpc的response对应的buf, 等待另外一个线程进行处理
  scoped_ptr<BytesBufferQueue> recv_buf_array_;
  scoped_ptr<Poco::FastMutex> mutex_recv_buf_array_;

  // 网络工作线程(reactor 运行的线程)
  scoped_ptr<Poco::Thread> net_worker_;

  // 处理Response 的工作线程
  scoped_ptr<Poco::Thread> response_worker_;
  scoped_ptr<Poco::Runnable> ra_response_;

  scoped_ptr<Poco::Net::SocketReactor> reactor_;
  scoped_ptr<Poco::Net::SocketAddress> address_;
  scoped_ptr<Poco::Net::StreamSocket> socket_;

  scoped_ptr<google::protobuf::Closure> on_reconnect_faild_cb_;

  void RemoveCanceledRpc(uint64 rpc_id);
  Poco::Net::StreamSocket* CreateSocket();
  void reg_reactor_handler(Poco::Net::StreamSocket* sock);
  void unreg_reactor_handler(Poco::Net::StreamSocket* sock);
  void onReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf);
  void onWritable(const Poco::AutoPtr<Poco::Net::WritableNotification>& pNf);
  void onShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& pNf);
  void onError(const Poco::AutoPtr<Poco::Net::ErrorNotification>& pNf);
  void process_response();
  void cancel_waiting_response_rpc(const std::string& reason);
  
  void on_socket_error();
  void auto_reconnect();
  
  DISALLOW_COPY_AND_ASSIGN(PocoRpcChannel);
};

} // namespace

#endif	/* POCORPCCHANNEL_H */


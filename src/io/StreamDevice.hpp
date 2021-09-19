class StreamDeviceBase {
};

class SocketDeviceBase: public StreamDeviceBase {
  public:
    virtual bool send(String address, String data, String end="\n") = 0;
    virtual String recv(int maxAmount) = 0;
};

class TransportDeviceBase {
  
};

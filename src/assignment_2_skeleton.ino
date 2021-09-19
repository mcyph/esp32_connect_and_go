void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

class NFCReaderWriterBase {
  
}

class DataWriter {
}

class JSONObject {
  public:
    JSONObject() {}
    
    virtual void add(String key, int i) = 0;
    virtual void add(String key, float i, decimalPlaces=2) = 0;
    virtual void add(String key, bool b) = 0;
    virtual void add(String key, String s) = 0;

    virtual String toString() = 0;
}

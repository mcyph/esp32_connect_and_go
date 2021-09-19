class FramebufferBase {
  
};

class ColorFramebufferBase: public FramebufferBase {
  public:
    virtual inline void setPixel(int x, int y, Color color) = 0;

  private:
    
};

class MonoFramebufferBase: public FramebufferBase {
  public:
    virtual inline void setPixel(int x, int y, char amount) = 0;

  private:
    
};

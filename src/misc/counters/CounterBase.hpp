class CounterBase {
  protected:
    long value;
    long minValue;
    long maxValue;

    bool canIncrementBy(long by) {
      if (by < 0) {
        return false;
      } else if (by > maxValue - minValue) {
        return false;
      } else {
        return true;
      }
    }
  
  public:
    CounterBase(long value, long minValue, long maxValue) {
      this->value = value;
      this->minValue = minValue;
      this->maxValue = maxValue;
    }

    //=======================================================
    // Getters
    //=======================================================

    long getValue() {
      return value;
    }

    float getFloatValue() {
      return (value - minValue) / float(maxValue - minValue);
    }

    double getDoubleValue() {
      return (value - minValue) / double(maxValue - minValue);
    }

    unsigned char getPercentValue() {
      // Some microcontrollers don't have native floating point capabilities 
      // (e.g. raspberry pi pico) - this returns between 0 and 100 for faster 
      // code in those cases
      return (value - minValue) * 100 / (maxValue - minValue);
    }

    //=======================================================
    // Setters/Modifiers
    //=======================================================

    // Must be overridden in subclasses
    virtual void increment(int by=1);
    virtual void decrement(int by=1);

    bool setValue(long v) {
      if (v < minValue || v > maxValue) {
        return false;
      } else {
        value = v;
        return true;
      }
    }
};


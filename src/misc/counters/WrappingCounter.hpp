#include "CounterBase.hpp"

class WrappingCounter: public CounterBase {
  public:
    WrappingCounter(long value, long minValue, long maxValue): CounterBase(value, minValue, maxValue) {}

    bool increment(long by=1) {
      if (!canIncrementBy(by)) {
        return false;
      }
      value += by;
      if (value > maxValue) {
          value -= maxValue - minValue;
      }
      return true;
    }

    bool decrement(long by=1) {
      if (!canIncrementBy(by)) {
        return false;
      }
      value -= by;
      if (value < minValue) {
          value += maxValue - minValue;
      }
      return true;
    }
};


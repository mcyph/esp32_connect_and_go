#include "CounterBase.hpp"

class BoundsCounter: public CounterBase {
  public:
    BoundsCounter(long value, long minValue, long maxValue): CounterBase(value, minValue, maxValue) {}

    bool increment(long by=1) {
      if (!canIncrementBy(by)) {
        return false;
      }
      value += by;
      if (value > maxValue) {
          value = maxValue;
      }
      return true;
    }

    bool decrement(long by=1) {
      if (!canIncrementBy(by)) {
        return false;
      }
      value -= by;
      if (value < minValue) {
          value = minValue;
      }
      return true;
    }
};

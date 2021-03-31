/*
  AverageValue
*/
#ifndef AverageValue_h
#define AverageValue_h

#include "Arduino.h"

class AverageValue {
public:
    AverageValue();

    void addValue(double nextValue);

    double getValue();

private:
    double value;
    int counter;
};

#endif
/*
  AverageValue.cpp 
*/

#include "Arduino.h"
#include "AverageValue.h"

AverageValue::AverageValue() {
    value = NULL;
    counter = 0;
}

void AverageValue::addValue(double nextValue) {
    if (value == NULL) {
        value = nextValue;
    } else {
        value = (value * counter + nextValue) / (counter + 1);
    }
    counter = counter + 1;
}

double AverageValue::getValue() {
    double result = value;
    value = NULL;
    counter = 0;
    return result;
}
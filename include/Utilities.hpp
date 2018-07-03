#ifndef UTILITIES_H
#define UTILITIES_H
#include <time.h>
#include <stdint.h>

using namespace std;

class Utilities
{
  public:
    static uint64_t getMicrotime();

  private:
    Utilities(){}; //Disallow creating an instance of this class
};

#endif //ifndef GPIOControl

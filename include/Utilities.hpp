#ifndef UTILITIES_H
#define UTILITIES_H
#include <time.h>
#include <stdint.h>
#include <sstream>

using namespace std;

class Utilities
{
  public:
    static uint64_t getMicrotime();

    template <typename T>
    static string to_string_with_precision(const T a_value, int n)
    {
        ostringstream out;
        out.precision(n);
        out << fixed << a_value;
        return out.str();
    }

  private:
    Utilities(){}; //Disallow creating an instance of this class
};

#endif //ifndef GPIOControl

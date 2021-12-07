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
    static void returnStringBetweenChars(string& original_string, string& new_string, string& delimiter);

    template <typename T>
    static string to_string_with_precision(const T a_value, int n)
    {
        ostringstream out;
        out.precision(n);
        out << fixed << a_value;
        return out.str();
    }

    template<class Container>
    static void split_string(const std::string &str, Container &cont, char delim)
    {
        std::size_t current, previous = 0;
        current = str.find(delim);
        while (current != std::string::npos) {
            cont.push_back(str.substr(previous, current - previous));
            previous = current + 1;
            current = str.find(delim, previous);
        }
        cont.push_back(str.substr(previous, current - previous));
    }

  private:
    Utilities(){}; //Disallow creating an instance of this class
};

#endif //ifndef GPIOControl

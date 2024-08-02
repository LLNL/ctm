#include <chrono>   // necessary for time measurement

// source: https://stackoverflow.com/questions/2808398/easily-measure-elapsed-time

template <class DT = std::chrono::milliseconds,
          class ClockT = std::chrono::steady_clock>
class Timer
{
    using timep_t = typename ClockT::time_point;
    timep_t _start = ClockT::now(), _end = {};

public:
    void tic() { 
        _end = timep_t{}; 
        _start = ClockT::now(); 
    }
    
    void toc() { _end = ClockT::now(); }
    
    template <class T = DT> 
    auto duration() const { 
        return std::chrono::duration_cast<T>(_end - _start); 
    }
};

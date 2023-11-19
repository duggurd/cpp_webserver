#include <iostream>
#include <ctime>

class timer {
    public:
    //init time object
    time_t rec_time;

    void start_timer() {
        //start and store start time
        rec_time = clock();
    }

    time_t stop_timer() {
        //get time, calc diff
        rec_time = clock() - rec_time;

        //return diff
        return rec_time;
    }
};
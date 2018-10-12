#include <b0/utils/time_sync.h>

#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <string>

namespace po = boost::program_options;

// unit-test for the time sync algorithm
// (does not cover time synchronization between nodes)

namespace b0
{

// adds an offset to hardware clock

class TimeSync_TEST : public TimeSync
{
public:
    TimeSync_TEST(int64_t offset, double speed, double max_slope) : TimeSync(max_slope), t0_(TimeSync::hardwareTimeUSec()), offset_(offset), speed_(speed) {}
    int64_t hardwareTimeUSec() const {return offset_ + speed_ * (TimeSync::hardwareTimeUSec() - t0_) + t0_;}
protected:
    int64_t t0_;
    int64_t offset_;
    double speed_;
};

}

inline int64_t sec(double s) { return s * 1000 * 1000; }

int main(int argc, char **argv)
{
    double max_slope = 0;
    int64_t offset = sec(0);
    double speed = 0;
    bool expected = false;
    b0::addOptions()
        ("max-slope,m", po::value<double>(&max_slope)->required(), "max slope")
        ("clock-speed,s", po::value<double>(&speed)->required(), "speed of test clock")
        ("expect-success,e", po::value<bool>(&expected)->required(), "expect success")
    ;
    b0::addPositionalOption("max-slope");
    b0::addPositionalOption("clock-speed");
    b0::addPositionalOption("expect-success");
    b0::init(argc, argv);

    b0::TimeSync c(max_slope);
    b0::TimeSync_TEST s(offset, speed, max_slope);

    int64_t test_end_time = c.hardwareTimeUSec() + sec(5);
    int64_t error = 0;

    while(c.hardwareTimeUSec() < test_end_time)
    {
        int64_t server_time = s.hardwareTimeUSec();
        c.updateTime(server_time);
        error = c.timeUSec() - server_time;
        std::cout << "server_time=" << server_time << ", error=" << error << std::endl;
        boost::this_thread::sleep_for(boost::chrono::seconds{1});
    }

    // if it will be able to track the clock, the final offset will be constant at approximately (1 - speed) seconds
    int64_t error_bound = sec((1 - speed) * 1.05 /* 5% tolerance */);
    return !!(expected ^ (std::abs(error) < std::abs(error_bound)));
}


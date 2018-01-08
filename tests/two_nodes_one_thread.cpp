#include <boost/thread.hpp>

#include <iostream>

#include "test_msgs.pb.h"
#include "resolver.pb.h"
#include <b0/resolver/resolver.h>
#include <b0/node.h>
#include <b0/exceptions.h>

void resolver_thread()
{
    b0::resolver::Resolver node;
    node.init();
    node.spin();
}

void node_thread()
{
    b0::Node n1("testnode-1"), n2("testnode-2");
    n1.init();
    try
    {
        n2.init();
    }
    catch(b0::exception::Exception &ex)
    {
        std::string expected_ex = "announceNode failed";
        if(ex.what() == expected_ex) exit(0);
    }
    exit(1);
}

void timeout_thread()
{
    boost::this_thread::sleep_for(boost::chrono::seconds{4});
    exit(2);
}

int main(int argc, char **argv)
{
    boost::thread t0(&timeout_thread);
    boost::thread t1(&resolver_thread);
    boost::this_thread::sleep_for(boost::chrono::seconds{1});
    boost::thread t2(&node_thread);
    t0.join();
}


#include <b0/node.h>
#include <b0/publisher.h>

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

/*! \example publisher_subscriber_oop/publisher_node_object.cpp
 * This is an example of creating a node by subclassing b0::Node.
 * Useful for overriding some node's behavior.
 */

//! \cond HIDDEN_SYMBOLS

class TestPublisherNode : public b0::Node
{
public:
    TestPublisherNode(std::string topic)
        : Node("publisher"),
          pub_(this, topic)
    {
    }

    void run()
    {
        int i = 0;
        while(!shutdownRequested())
        {
            spinOnce();

            std::string msg = (boost::format("msg-%d") % i++).str();
            std::cout << "Sending: " << msg << std::endl;
            pub_.publish(msg);

            sleepUSec(1000000);
        }
    }

private:
    b0::Publisher pub_;
};

int main(int argc, char **argv)
{
    b0::addOptions()
        ("topic", po::value<std::string>()->default_value("A"), "topic name")
    ;
    b0::init(argc, argv);
    TestPublisherNode node(b0::getOption("topic").as<std::string>());
    node.init();
    node.run();
    node.cleanup();
    return 0;
}

//! \endcond


#ifndef B0__SUBSCRIBER_H__INCLUDED
#define B0__SUBSCRIBER_H__INCLUDED

#include <string>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <b0/socket.h>

namespace b0
{

class Node;

/*!
 * \brief The subscriber class
 *
 * This class wraps a SUB socket. It will automatically connect to the
 * XPUB socket of the proxy (note: the proxy is started by the resolver node).
 *
 * In order to receive some message, you must set a topic subscription with Subscriber::subscribe.
 * You can set multiple subscription, and the incoming messages will match any of those.
 *
 * The subscription topics are strings and are matched on a prefix basis.
 * (i.e. if the topic of the incoming message is "AAA", a subscription for "A" will match it)
 *
 * \sa b0::Publisher, b0::Subscriber
 */
class Subscriber : public Socket
{
public:
    using logger::LogInterface::log;

    //! \cond HIDDEN_SYMBOLS

    // needed to disambiguate the overloaded constructors
    struct CallbackWithoutType {};
    struct CallbackWithType {};

    //! \endcond

    /*!
     * \brief Construct an Subscriber child of the specified Node, optionally using a boost::function as callback
     */
    Subscriber(Node *node, std::string topic_name, boost::function<void(const std::string&)> callback = 0, bool managed = true, bool notify_graph = true)
        : Subscriber(node, topic_name, CallbackWithoutType(), callback, managed, notify_graph)
    {}

    /*!
     * \brief Construct an Subscriber child of the specified Node, optionally using a boost::function as callback
     */
    Subscriber(Node *node, std::string topic_name, const CallbackWithoutType &_, boost::function<void(const std::string&)> callback = 0, bool managed = true, bool notify_graph = true);

    /*!
     * \brief Construct an Subscriber child of the specified Node, optionally using a boost::function as callback
     */
    Subscriber(Node *node, std::string topic_name, const CallbackWithType &_, boost::function<void(const std::string&, const std::string&)> callback = 0, bool managed = true, bool notify_graph = true);

    /*!
     * \brief Subscriber destructor
     */
    virtual ~Subscriber();

    /*!
     * \brief Log a message using node's logger, prepending this subscriber informations
     */
    void log(LogLevel level, std::string message) const override;

    /*!
     * \brief Perform initialization and optionally send graph notify
     */
    virtual void init() override;

    /*!
     * \brief Perform cleanup and optionally send graph notify
     */
    virtual void cleanup() override;

    /*!
     * \brief Process incoming messages and call callbacks
     */
    virtual void spinOnce() override;

    /*!
     * \brief Return the name of this subscriber's topic
     */
    std::string getTopicName();

protected:
    /*!
     * \brief Connect to the remote address
     */
    virtual void connect();

    /*!
     * \brief Disconnect from the remote address
     */
    virtual void disconnect();

    //! If false this socket will not send announcement to resolv (i.e. it will be "invisible")
    const bool notify_graph_;

    /*!
     * \brief Callback which will be called when a new message is read from the socket
     */
    boost::function<void(const std::string&)> callback_;

    /*!
     * \brief Callback which will be called when a new message is read from the socket
     */
    boost::function<void(const std::string&, const std::string&)> callback_with_type_;
};

} // namespace b0

#endif // B0__SUBSCRIBER_H__INCLUDED

#pragma once

#include <boost/asio.hpp>
#include <functional>
#include <string>
#include "transaction.h"

class NetworkClient {
public:
    using SuccessHandler = std::function<void(const std::string&)>;
    using ErrorHandler = std::function<void(const std::string&)>;

    explicit NetworkClient(boost::asio::io_context& io);

    void connect(const std::string& host, unsigned short port,
                 std::function<void()> onConnected,
                 ErrorHandler onError);

    void sendTransaction(const Transaction& tx,
                         SuccessHandler onSuccess,
                         ErrorHandler onError);

    void getAllTransactions(SuccessHandler onSuccess,
                            ErrorHandler onError);

    void close();

private:
    void readResponse(SuccessHandler onSuccess, ErrorHandler onError);

    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf responseBuffer_;
};
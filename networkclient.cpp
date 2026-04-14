#include "NetworkClient.h"
#include "JsonTools.h"
#include <memory>
#include <istream>

using boost::asio::ip::tcp;

NetworkClient::NetworkClient(boost::asio::io_context& io)
    : resolver_(io), socket_(io) {}

void NetworkClient::connect(const std::string& host,
                            unsigned short port,
                            std::function<void()> onConnected,
                            ErrorHandler onError) {
    resolver_.async_resolve(
        host,
        std::to_string(port),
        [this, onConnected, onError](const boost::system::error_code& ec,
                                     tcp::resolver::results_type results) {
            if (ec) {
                onError("Resolve failed: " + ec.message());
                return;
            }

            boost::asio::async_connect(
                socket_,
                results,
                [onConnected, onError](const boost::system::error_code& ec,
                                       const tcp::endpoint&) {
                    if (ec) {
                        onError("Connect failed: " + ec.message());
                    } else {
                        onConnected();
                    }
                });
        });
}

void NetworkClient::sendTransaction(const Transaction& tx,
                                    SuccessHandler onSuccess,
                                    ErrorHandler onError) {
    auto message = std::make_shared<std::string>(JsonTools::addTransactionMsg(tx));

    boost::asio::async_write(
        socket_,
        boost::asio::buffer(*message),
        [this, message, onSuccess, onError](const boost::system::error_code& ec, std::size_t) {
            if (ec) {
                onError("Send failed: " + ec.message());
                return;
            }
            readResponse(onSuccess, onError);
        });
}

void NetworkClient::getAllTransactions(SuccessHandler onSuccess,
                                       ErrorHandler onError) {
    auto message = std::make_shared<std::string>(JsonTools::getAllMsg());

    boost::asio::async_write(
        socket_,
        boost::asio::buffer(*message),
        [this, message, onSuccess, onError](const boost::system::error_code& ec, std::size_t) {
            if (ec) {
                onError("Request failed: " + ec.message());
                return;
            }
            readResponse(onSuccess, onError);
        });
}

void NetworkClient::readResponse(SuccessHandler onSuccess,
                                 ErrorHandler onError) {
    boost::asio::async_read_until(
        socket_,
        responseBuffer_,
        '\n',
        [this, onSuccess, onError](const boost::system::error_code& ec, std::size_t) {
            if (ec) {
                onError("Read failed: " + ec.message());
                return;
            }

            std::istream input(&responseBuffer_);
            std::string line;
            std::getline(input, line);
            onSuccess(line);
        });
}

void NetworkClient::close() {
    boost::system::error_code ec;
    socket_.close(ec);
}
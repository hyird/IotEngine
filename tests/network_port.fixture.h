#pragma once

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/ip/udp.hpp>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace test {

inline std::uint16_t reserveTcpUdpPort() {
    asio::io_context context;
    std::error_code error;
    std::uint16_t port = 0;
    // Windows excludes different, sometimes adjacent port ranges for TCP and UDP.
    // Probe both protocols and allow a full default ephemeral range of attempts.
    for (std::size_t attempt = 0; attempt < 16384; ++attempt) {
        asio::ip::udp::socket udp(context);
        udp.open(asio::ip::udp::v4(), error);
        if (error) continue;
        udp.bind({asio::ip::address_v4::loopback(), 0}, error);
        if (error) continue;
        port = udp.local_endpoint(error).port();
        if (error) continue;
        asio::ip::tcp::acceptor tcp(context);
        tcp.open(asio::ip::tcp::v4(), error);
        if (error) continue;
        tcp.bind({asio::ip::address_v4::loopback(), port}, error);
        if (!error) return port;
    }
    throw std::runtime_error("could not reserve a shared TCP/UDP loopback port; last port=" +
        std::to_string(port) + ", error=" + error.message());
}

} // namespace test

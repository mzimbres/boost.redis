/* Copyright (c) 2018-2022 Marcelo Zimbres Silva (mzimbres@gmail.com)
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE.txt)
 */

#include <boost/redis/connection.hpp>
#include <boost/asio/deferred.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/consign.hpp>
#include <iostream>
#include "protobuf.hpp"

// See the definition in person.proto. This header is automatically
// generated by CMakeLists.txt.
#include "person.pb.h"

#if defined(BOOST_ASIO_HAS_CO_AWAIT)

namespace net = boost::asio;
using boost::redis::request;
using boost::redis::response;
using boost::redis::operation;
using boost::redis::ignore_t;
using boost::redis::config;
using connection = net::deferred_t::as_default_on_t<boost::redis::connection>;

// The protobuf type described in examples/person.proto
using tutorial::person;

// Boost.Redis customization points (examples/protobuf.hpp)
namespace tutorial
{

void boost_redis_to_bulk(std::string& to, person const& u)
   { boost::redis::protobuf::to_bulk(to, u); }

void boost_redis_from_bulk(person& u, std::string_view sv, boost::system::error_code& ec)
   { boost::redis::protobuf::from_bulk(u, sv, ec); }

} // tutorial

using tutorial::boost_redis_to_bulk;
using tutorial::boost_redis_from_bulk;

net::awaitable<void> co_main(config cfg)
{
   auto ex = co_await net::this_coro::executor;
   auto conn = std::make_shared<connection>(ex);
   conn->async_run(cfg, {}, net::consign(net::detached, conn));

   person p;
   p.set_name("Louis");
   p.set_id(3);
   p.set_email("No email yet.");

   request req;
   req.push("SET", "protobuf-key", p);
   req.push("GET", "protobuf-key");

   response<ignore_t, person> resp;

   // Sends the request and receives the response.
   co_await conn->async_exec(req, resp);
   conn->cancel();

   std::cout
      << "Name: " << std::get<1>(resp).value().name() << "\n"
      << "Age: " << std::get<1>(resp).value().id() << "\n"
      << "Email: " << std::get<1>(resp).value().email() << "\n";
}

#endif // defined(BOOST_ASIO_HAS_CO_AWAIT)

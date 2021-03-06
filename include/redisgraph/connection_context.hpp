/**
* Copyright 2019 RRD Software Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/
#ifndef REDIS_CONN_CONTEXT_H
#define REDIS_CONN_CONTEXT_H
#include <cstdint>
#include <string>
#include <type_traits>

// here we apply the rule of zero.
namespace redisgraph {
class connection_context 
{
public:

    connection_context() = default;
    /** Constructor
    * @param host           Hostname
    * @param redis_port     Port
    * @param n_threads      Number of active threads
    * @param pool_size      Dimension of connection pool
    */
    explicit connection_context(const std::string& host,
        const uint16_t& redis_port = 6379, const size_t n_threads = 4, const size_t pool_size = 10): 
        redis_host_(host), redis_port_(redis_port), n_threads_(n_threads), pool_size_(pool_size)
    {
    }
    connection_context(const connection_context& context)
    {
        redis_host_ = context.redis_host_;
        redis_port_ = context.redis_port_;
        n_threads_ = context.n_threads_;
        pool_size_ = context.pool_size_;
    }
    connection_context(const connection_context&& context)
    {
        redis_host_ = std::move(context.redis_host_);
        redis_port_ = context.redis_port_;
        n_threads_ = context.n_threads_;
        pool_size_ = context.pool_size_;
    }
    connection_context& operator=(const connection_context& context)
    {
        if (&context != this)
        {
            redis_host_ = context.redis_host_;
            redis_port_ = context.redis_port_;
            n_threads_ = context.n_threads_;
            pool_size_ = context.pool_size_;
        }
        return *this;
    }
    ~connection_context() = default;

    /** 
    * Return the connection host
    */
    std::string host() const
    {
        return redis_host_;
    }
    /**
    *  Return the connection port
    */
    uint16_t port() const
    {
        return redis_port_;
    }
    /**
    * Return the number of threads
    */
    size_t n_threads() const
    {
        return n_threads_;
    }
    /**
    * Return the pool size.
    */
    size_t pool_size() const
    {
        return pool_size_;
    }
private :
  std::string redis_host_;
  uint32_t redis_port_;
  size_t n_threads_;
  size_t pool_size_;
};
} // namespace redisgraph
#endif

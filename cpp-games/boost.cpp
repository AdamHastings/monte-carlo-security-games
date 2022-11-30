#include <iostream>
#include <mutex>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

std::mutex output_mutex;

int main()
{
    boost::asio::thread_pool workers(16);

    for(int n = 0; n < 16; ++n)
    {
        boost::asio::post(workers, [=]
        {
            auto guard = std::lock_guard(output_mutex);
            std::cout << "Hello from task " << n << "!\n";
        });
    }

    workers.wait();

    return 0;
}
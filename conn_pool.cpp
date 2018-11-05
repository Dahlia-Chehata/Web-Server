#include "conn_pool.h"

conn_pool::conn_pool() : requests(), workers(), queue_mutex(), condition(),
                         accept_requests(true)
{
    
}

conn_pool::~conn_pool()
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    accept_requests = false;
    lock.unlock();

    condition.notify_all();
    for(conn_worker &worker: workers)
        worker.join();
}

void conn_pool::enqueue(std::function<void()> req)
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    requests.push(req);
    lock.unlock();
    condition.notify_one();
}

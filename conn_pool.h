#ifndef YAWS_CONNECTION_POOL_H_
#define YAWS_CONNECTION_POOL_H_

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

#include "conn_worker.h"

class conn_pool
{
private:
    // need to keep track of workers so we can join them.
    std::vector<conn_worker> workers;

    // the requests a conn_workers will work on.
    std::queue< std::function<void()> > requests;

    // synchronization tools
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool accept_requests;
public:
    conn_pool(/* args */);
    ~conn_pool();


    void enqueue(std::function<void()> func);
};


#endif // YAWS_CONNECTION_POOL_H_

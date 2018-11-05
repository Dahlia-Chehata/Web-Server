#ifndef YAWS_CONNECTION_WORKER_H_
#define YAWS_CONNECTION_WORKER_H_

#include <thread>

class conn_worker
{
private:
    std::thread t;
public:
    conn_worker(/* args */);
    ~conn_worker();

    void join();
};

#endif // YAWS_CONNECTION_WORKER_H_

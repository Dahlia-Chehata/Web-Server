#include "conn_worker.h"


conn_worker::conn_worker(/* args */)
{

}

conn_worker::~conn_worker()
{

}

void conn_worker::join()
{
    t.join();
}

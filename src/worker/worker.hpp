#ifndef _WORKER_HPP_
#define _WORKER_HPP_

#include <cstddef>
#include <sys/select.h>
#include <fstream>
#include <sstream>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include "Data.hpp"
#include "Global.hpp"

#define MAX_NB_CONNECTION        32768
bool workerLoopCoordinator (Data &data);

extern pthread_mutex_t g_accept_mutex;
extern pthread_mutex_t g_connection_mutex;

#endif // _WORKER_HPP_

#include "worker.hpp"
#include "Handler.hpp"

int g_nb_connections = 0;
pthread_mutex_t g_accept_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_connection_mutex = PTHREAD_MUTEX_INITIALIZER;

void *threadWorker (void *arg)
{
  int connection_fd;
  socklen_t size (sizeof (SOCKADDR_IN));
  Data *data (reinterpret_cast<Data *>(arg));
  Handler h;

  int max;
  int internalFds[6];
  int toCloseFds[6];

  fd_set selectedFDs[3]; // read[0] write[1] except[2]

  std::list<int> thr_connection_fds;
  std::list<int> response_fds;

  while (true)
	{
	  //if (data->connection_fds.size () >= MAX_NB_CONNECTION) //ddos
	  //continue;
	  // Request Handler
	  for (std::list<int>::iterator it (thr_connection_fds.begin ());
		   it != thr_connection_fds.end (); ++it)
		{
		  for (int i (0); i < 6; i++)
			{
			  internalFds[i] = -1;
			  toCloseFds[i] = -1;
			}
		  bool ret;
		  try
			{
			  ret = h.signal (*it, selectedFDs, internalFds, toCloseFds);
			}
		  catch (std::exception &e)
			{
			  	h.cleanData (*it);
				close (*it);
				pthread_mutex_lock (&g_connection_mutex);
				g_nb_connections -= 1;
				pthread_mutex_unlock (&g_connection_mutex);
				h.eraseRequest (*it);
				it = thr_connection_fds.erase (it);
				if (it != thr_connection_fds.end ()
					&& it != thr_connection_fds.begin ())
					--it;
				if (it == thr_connection_fds.end ())
					break;

			}
		  if (ret)
			{
			  bool toErase;
			  toErase = h.cleanData (*it);
			  if (toErase)
				{
				  close (*it);
				  pthread_mutex_lock (&g_connection_mutex);
				  g_nb_connections -= 1;
				  pthread_mutex_unlock (&g_connection_mutex);
				  h.eraseRequest (*it);
				  it = thr_connection_fds.erase (it);
				  if (it != thr_connection_fds.end ()
					  && it != thr_connection_fds.begin ())
					--it;
				  if (it == thr_connection_fds.end ())
					break;
				}
			}
		  for (int i (0); i < 6; i++)
			{
			  if (internalFds[i] < 0)
				continue;
			  try
				{
				  response_fds.push_back (internalFds[i]);
				}
			  catch (std::exception &e)
				{
				  for (int j = 0; j < i; ++j)
					close (internalFds[i]);
				}
			}
		  for (int i (0); i < 6; ++i)
			{
			  for (std::list<int>::iterator it2 (response_fds.begin ());
				   it2 != response_fds.end (); ++it2)
				{
				  if (*it2 == toCloseFds[i])
					{
					  response_fds.erase (it2);
					  break;
					}
				}
			}
		}
	  // Clear flags
	  FD_ZERO (&selectedFDs[0]);
	  FD_ZERO (&selectedFDs[1]);
	  FD_ZERO (&selectedFDs[2]);
	  // Add all socket_fd in non blocking mode (for accept) into set to select
	  for (std::vector<int>::iterator it (data->socket_fds.begin ());
		   it != data->socket_fds.end (); ++it)
		{
		  FD_SET (*it, &selectedFDs[0]);
		}
	  // Add all thread_fd in non blocking mode (for Read) into set to select
	  for (std::list<int>::iterator it (thr_connection_fds.begin ());
		   it != thr_connection_fds.end (); ++it)
		{
		  FD_SET (*it, &selectedFDs[0]);
		  if (h.shouldWriteSelect (*it))
			FD_SET (*it, &selectedFDs[1]);
		  //FD_SET (*it, &eFds);
		  //fcntl(fd, F_SETFL, O_NONBLOCK);
		}
	  for (std::list<int>::iterator it (response_fds.begin ());
		   it != response_fds.end (); ++it)
		{
		  FD_SET (*it, &selectedFDs[0]);
		  FD_SET (*it, &selectedFDs[1]);
		  //fcntl(*it, F_SETFL, O_NONBLOCK);
		}
	  // Select should never fail, nfds as max_fd from thr_connection_fds + 1
	  max = 0;
	  if (max_element (thr_connection_fds.begin (), thr_connection_fds.end ())
		  != thr_connection_fds.end ())
		max = *max_element (thr_connection_fds.begin (), thr_connection_fds.end ());
	  if (max_element (response_fds.begin (), response_fds.end ())
		  != response_fds.end ())
		max = std::max (max, *max_element (response_fds.begin (), response_fds.end ()));
	  max =
		  std::max (max, *max_element (data->socket_fds.begin (), data->socket_fds.end ()))
		  + 1;
	  struct timeval timeout; // TO allow the loop check of alive and avoid slowloris + incomplete connections
	  timeout.tv_sec = 31;
	  timeout.tv_usec = 0;
	  if (select (max, &selectedFDs[0], &selectedFDs[1], &selectedFDs[2], &timeout)
		  < 0)
		{
		  std::list<int> v = thr_connection_fds;
		  v.insert (v.end (), response_fds.begin (), response_fds.end ());
		  for (std::list<int>::iterator it = v.begin (); it != v.end (); ++it)
			{
			  struct stat s;
			  if (fstat (*it, &s) < 0)
				std::cout << *it << std::endl;
			}
		  std::cout << "SELECT FAILURE: " << strerror (errno) << std::endl;
		  continue;
		}
	  // Next lines for leak tests
	  fcntl (STDIN_FILENO, F_SETFL, O_NONBLOCK);
	  char buf[1];
	  if (read (STDIN_FILENO, buf, 1) > 0)
		{
		  return NULL;
		}
	  for (size_t i (0); i < data->socket_fds.size (); i++)
		{
		  if (g_nb_connections > 165)
			break;
		  // IF FD_ISSET socket, tu l'acceptes pour eviter de loop en permanence
		  if (!FD_ISSET (data->socket_fds[i], &selectedFDs[0]))
			continue;
		  pthread_mutex_lock (&g_accept_mutex);
		  connection_fd = accept (data->socket_fds[i],
								  reinterpret_cast<SOCKADDR *>(&data->serverAddrs[i]), &size);
		  if (connection_fd < 0)
			{
			  pthread_mutex_unlock (&g_accept_mutex);
			  continue;
			}
		  Request rq (data->servers);
		  try
			{
			  h.AddRequest (rq, connection_fd, data->servers[i].getListenPort ());
			  pthread_mutex_lock (&g_connection_mutex);
			  g_nb_connections += 1;
			  pthread_mutex_unlock (&g_connection_mutex);
			  thr_connection_fds.push_back (connection_fd);
			}
		  catch (std::exception &e)
			{
				continue;
			}
		  pthread_mutex_unlock (&g_accept_mutex);
		  if (g_verbose)
			{
			  std::cout << "Connection_fd = " << connection_fd << std::endl;
			  std::cout << "Data->connection_fds = "
						<< data->connection_fds.size ()
						<< std::endl;
			  std::cout << "Data->socket_fds = " << data->socket_fds.size ()
						<< std::endl;
			  std::cout << "thr_connection_fds = " << thr_connection_fds.size ()
						<< std::endl;
			  std::cout << std::endl;
			}
		}
	}
  return NULL;
}

// Launch worker threads
bool workerLoopCoordinator (Data &data)
{
  pthread_t accept_thr[data.servers.getThrNb ()];

  if (g_vverbose)
	std::cout << "WORKER_LOOP_COORDINATOR" << std::endl;
  // Setup ddos Mutexes
  for (size_t i (0); i < data.servers.getThrNb (); i++)
	{
	  if (g_vverbose)
		std::cout << "\tThread " << i + 1 << std::endl;
	  if (pthread_create (&accept_thr[i], NULL, &threadWorker, &data))
		return false;
	}
  if (g_vverbose)
	{
	  std::cout << std::endl;
	  std::cout << "---------------" << std::endl;
	}
  for (size_t i (0); i < data.servers.getThrNb (); i++)
	{
	  pthread_join (accept_thr[i], NULL);
	}
  return true;
}

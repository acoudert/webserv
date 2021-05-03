#include <mimes.hpp>
#include "main.hpp"

bool g_verbose = false;
bool g_vverbose = false;
bool g_slowloris = false;
bool g_timeout = false;
bool g_pipe = false;

void displayMain (Data const &data)
{
  std::cout << "MAIN" << std::endl;
  std::cout << "\tdata.thr_nb: " << data.servers.getThrNb () << std::endl;
  std::cout << "\tdata.socket_fds.size(): " << data.socket_fds.size ()
			<< std::endl;
  for (size_t i (0); i < data.socket_fds.size (); i++)
	std::cout << "\t\tdata.socket_fds[" << i << "]: " << data.socket_fds[i]
			  << std::endl;
  std::cout << "\tdata.serverAddrs.size(): " << data.serverAddrs.size ()
			<< std::endl;
  for (size_t i (0); i < data.serverAddrs.size (); i++)
	std::cout << "\t\tdata.serverAddrs[" << i << "].sin_port: " \
 << ntohs (data.serverAddrs[i].sin_port) << std::endl;
  std::cout << std::endl;
}

// Set up struct sockaddr_in up to listening state included for each server
bool setUpSocket (Data &data)
{
  int one;
  int socket_fd;
  SOCKADDR_IN serverAddr;

  signal (SIGPIPE, sigPipeHandler);
  for (size_t i (0); i < data.servers.getSize (); i++)
	{
	  if ((socket_fd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
		return false;
	  fcntl (socket_fd, F_SETFL, O_NONBLOCK);
	  one = 1;
	  if (setsockopt (socket_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof (int))
		  < 0)
		return false;
	  serverAddr.sin_family = AF_INET;
	  serverAddr.sin_port = htons (data.servers[i].getListenPort ());
	  serverAddr.sin_addr.s_addr = htonl (INADDR_ANY); // Even if htonl has no effect with INADDR_ANY
	  if (bind (socket_fd, reinterpret_cast<SOCKADDR *>(&serverAddr), sizeof (SOCKADDR_IN))
		  < 0)
		return false;
	  if (listen (socket_fd, MAX_NB_CONNECTION)
		  < 0) // May require to increase MAX_NB_CONNECTION value to meet subject requirements
		return false;
	  data.socket_fds.push_back (socket_fd);
	  data.serverAddrs.push_back (serverAddr);
	}
  return true;
}

int main (int ac, char **av)
{
  initMimes ();
  Data data (av);

  (void) ac;
  if (data.servers.errorConf ())
	return error ("Server Configuration File", SERVER_CONF);
  if (!(setUpSocket (data)))
	return error (strerror (errno), errno);
  //registerSocket(data.socket_fds);
  //signal(SIGINT, &sigIntHandler);
  if (g_vverbose)
	{
	  data.servers.displayServerGroup ();
	  displayMain (data);
	}
  if (!(workerLoopCoordinator (data)))
	{
	  return error (strerror (errno), errno);
	}
  if (mimes)
	{
	  delete[] mimes;
	}
  return 0;
}

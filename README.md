# Open Fix
Open fix is a combination of components to connect to a FIX server and perform operations on FIX messages. Has a TCP client/server, metrics server, FIX parser and support for client configurations.
## Modules - Incomplete
- **session.h**: basic wrapper around sockets; write, read and close operations
- **tcp_client.h**: Implements a simple polling TCP client, a Handler class must be passed in the templated arguments to handle read operations
`bool operator( )( net::tcp_session *session, char *buf, size_t len )` must be defined within the handler class, if it returns false, the buffer used for the read operations
is not pushed back into the pool. The push method in the class adds a message to the outgoing message atomic queue.
```c++
template < size_t BufSize, size_t BufpoolSize, size_t OutSize, class Handler >
	class tcp_client_impl : public tcp_session
```
- **tcp_server.h**: Implements a simple polling TCP server, inherits tcp_session which represents the listening file descriptor
```c++
template < int MaxClients, int OutSize, int BufSize, int BufPoolSize, class Handler >
	class tcp_server_impl : public tcp_session
```
- **details.h**: Miscellaneous utilities
- **fix_parse.h**: A minimalistic FIX protocol parser inspired by [hffix](https://github.com/jamesdbrock/hffix)
- **tcsns.h**: [TCSNS](https://github.com/MengRao/tscns)
- **message.h**: Simple TCP message structure
- **fix_client.h**: Implements a barebones FIX TCP client inherited by the fix_client class that uses a Broker class to handle different FIX message types.
```c++
constexpr size_t buffer_size = 2048;
using fix_tcp_client = net::tcp_client_impl< buffer_size, 64, 64, fix_handler< 64 > >;
template < class Broker > class fix_client : public fix_tcp_client
```
## Docker
The docker folder contains docker image configurations for prometheus and grafana, its used for monitoring purposes.
Prometheus is used to collect time series data about the average queue latency, messages dropped and extra valuable information in the case of large scale deployment, 
Grafana is used to prettify all the data and have a single dashboard where everything is monitored.
## Compiling
```
git clone https://github.com/auth12/open_fix.git
cd open_fix
mkdir build
cd build
cmake ..
```

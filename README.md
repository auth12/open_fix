# Open Fix
Open fix implements multiple components to connect to a FIX server.
## Compiling
```
git clone https://github.com/auth12/open_fix.git
cd open_fix
mkdir build
cd build
cmake ..
```
## Usage
### TCP Client
The TCP Client inherits `tcp_session` which contains the file descriptor of the connection, the client can be easily configured for any use type from the templated arguments:
- `BufSize`: The size of a single buffer in the buffer pool.
- `BufPoolSize`: Max elements in the pool.
- `OutSize`: Max elements in the outgoing message queue, outgoing message can be pushed using `void post( const net::tcp_msg_t &msg )`
- `Handler`: The handler class must contain a bool operator called after a new incoming message is read, if the operator returns true, the buffer is automatically
put back in the pool, otherwise it is up to you to free it back through the `void release_buf( char *buf )` method. Example of its use can be found in the [fix client](https://github.com/auth12/open_fix/blob/8246ff5020a836f8ce01086e7e57137b2db4268a/include/fix_client.h#L24) implementation. Example use of the handler is if you want to handle incoming messages in a different thread rather than in serial, push the incoming messages into an atomic queue and handle them elsewhere.
### TCP Server
Same design as TCP Client, the differences being: `tagged_tcp_msg_t` is used for outgoing messages to support sending messages separately to clients, if `fd` is set to `0`, the message is broadcasted to all connected clients. `tcp_session` holds the listening socket.
- `MaxClients`
- `OutSize`: Max elements in the outgoing message queue.
- `BufSize`: The size of a single buffer in the buffer pool.
- `BufPoolSize`: Max elements in the pool.
- `Handler`: The handler class for the server must define multiple methods to handle different operations, an instance of the server is passed to the handler's constructor.
```c++
struct server_handler_t {
	tcp_server *srv;

	tcp_handler_t( tcp_server *ptr ) : srv{ ptr } { };

	void on_connect( int fd ) {}
	bool on_out( const net::tagged_tcp_msg_t &msg ) {}
	bool on_read( int fd, char *buf, size_t len ) {}
	void on_disconnect( int fd ) {}
};
```
`bool on_out( const net::tagged_tcp_msg_t &msg )` is a filter called before a message is sent to a specific client
### FIX Parser
```c++
fix::fix_message_t msg{ fix_buf };
msg.init( );
if( !msg.valid ) {
	spdlog::error( "invalid msg" );
	return 0;
}

spdlog::info( "Checksum: {}, msg type: {}", msg.checksum, msg.type );
for ( auto &f : msg ) {
	spdlog::info( "{}->{}", f.tag, f.val.as_string( ) );
}
```
### FIX Client
The FIX client is built as a handler class for the TCP client. As seen [here](https://github.com/auth12/open_fix/blob/8246ff5020a836f8ce01086e7e57137b2db4268a/include/fix_client.h#L24) incoming messages are checked for any loss and then pushed sequentially into an atomic queue. The `fix_client` class inherits `fix_tcp_client` and a `Broker` class must be passed to handle broker related information such as current session state, LogOn, Heartbeat and MarketData message creation, the `bool consume_fix_msg( fix_msg_t &msg )` attempts to consume a FIX message from the queue and returns true when a message was popped. [LMAX example implementation](https://github.com/auth12/open_fix/blob/8246ff5020a836f8ce01086e7e57137b2db4268a/include/lmax.h)
```c++
struct broker_t {
	std::string fix_ver, target_id, sender_id;
	std::atomic_int seq, state;

	lmax_t( ) : seq{ 1 } {}

	int next_seq( ) { return seq.fetch_add( 1, std::memory_order_release ); }

	size_t create_logon( char *buf, const size_t buf_size ) {}

	size_t create_heartbeat( char *buf, const size_t buf_size ) {}

	size_t create_market_request( char *buf, const size_t buf_size ) {}
};
```
## Docker
The docker folder contains docker image configurations for prometheus and grafana, its used for monitoring purposes.
Prometheus is used to collect time series data about the average queue latency, messages dropped and extra valuable information in the case of large scale deployment, 
Grafana is used to prettify all the data and have a single dashboard where everything is monitored.
## Modules
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
  

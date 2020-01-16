#pragma once

struct errdata_t{
	int	eno;					// エラーコード
	char* errmsg;				// エラーメッセージ
};

struct errdata_t errdata[] = {

WSANOTINITIALISED,
   	"A successful WSAStartup must occur before using this function."
,WSAENETDOWN,	
   	"The Windows Sockets implementation has detected that the network "
	"subsystem has failed."
,WSAENOTCONN,	
	"The socket is not connected."
,WSAEINTR,	
	"The (blocking) call was canceled using WSACancelBlockingCall."
,WSAEINPROGRESS,	
	"A blocking Windows Sockets operation is in progress."
,WSAENOTSOCK,	
	"The descriptor is not a socket."
,WSAEOPNOTSUPP,	
	"MSG_OOB was specified, but the socket is not of type SOCK_STREAM."
,WSAESHUTDOWN,	
	"The socket has been shut down; it is not possible to recv on a socket"
	" after shutdown has been invoked with how set to 0 or 2."
,WSAEWOULDBLOCK,	
	"The socket is marked as nonblocking and the receive operation would block."
,WSAEMSGSIZE,	
	"The datagram was too large to fit into the specified buffer and was truncated."
,WSAEINVAL,	
	"The socket has not been bound with bind."
,WSAECONNABORTED,	
	"The virtual circuit was aborted due to timeout or other failure."
,WSAECONNRESET,	
	"The virtual circuit was reset by the remote side."
,WSANOTINITIALISED,	
	"A successful WSAStartup must occur before using this function."
,WSAENETDOWN,	
	"The Windows Sockets implementation has detected that the network"
	" subsystem has failed."
,WSAEAFNOSUPPORT,	
	"The specified address family is not supported."
,WSAEINPROGRESS,	
	"A blocking Windows Sockets operation is in progress."
,WSAEMFILE,	
	"No more file descriptors are available."
,WSAENOBUFS,	
	"No buffer space is available. The socket cannot be created."
,WSAEPROTONOSUPPORT,	
	"The specified protocol is not supported."
,WSAEPROTOTYPE,	
	"The specified protocol is the wrong type for this socket."
,WSAESOCKTNOSUPPORT,	
	"The specified socket type is not supported in this address family."
,WSANOTINITIALISED,	
	"A successful WSAStartup must occur before using this function."
,WSAENETDOWN,	
	"The Windows Sockets implementation has detected that the "
	"network subsystem has failed."
,WSAEADDRINUSE, 	
	"The specified address is already in use. (See the SO_REUSEADDR"
	" socket option under setsockopt.)"
,WSAEFAULT,	
	"The namelen argument is too small (less than the size of a struct sockaddr)."
,WSAEINPROGRESS,	
	"A blocking Windows Sockets call is in progress."
,WSAEAFNOSUPPORT,	
	"The specified address family is not supported by this protocol."
,WSAEINVAL,	
	"The socket is already bound to an address."
,WSAENOBUFS,	
	"Not enough buffers available, too many connections."
,WSAENOTSOCK,	
	"The descriptor is not a socket."
,WSANOTINITIALISED,
	"A successful WSAStartup must occur before using this function."
,WSAENETDOWN,	
	"The Windows Sockets implementation has detected that "
	"the network subsystem has failed."
,WSAEINVAL,	
	"Indicates that one of the specified parameters was invalid."
,WSAEINPROGRESS,	
	"A blocking Windows Sockets operation is in progress."
,WSAEADDRINUSE,	
	"The specified address is already in use."
,WSAEADDRNOTAVAIL,	
	"The specified address is not available from the local computer."
,WSAEAFNOSUPPORT,	
	"Addresses in the specified family cannot be used with this socket."
,WSAECONNREFUSED,	
	"The attempt to connect was forcefully rejected."
,WSAEDESTADDRREQ,	
	"A destination address is required."
,WSAEFAULT,	
	"The namelen argument is incorrect."
,WSAEINVAL,	
	"The socket is already bound to an address."
,WSAEISCONN,	
	"The socket is already connected."
,WSAEMFILE,	
	"No more file descriptors are available."
,WSAENETUNREACH,	
	"The network can't be reached from this host at this time."
,WSAENOBUFS,	
	"No buffer space is available. The socket cannot be connected."
,WSAENOTCONN,	
	"The socket is not connected."
,WSAENOTSOCK,	
	"The descriptor is a file, not a socket."
,WSAETIMEDOUT,	
	"Attempt to connect timed out without establishing a connection"
,WSAENETDOWN,	
	"The Windows Sockets implementation has detected that the network"
	" subsystem has failed."
,WSAECONNRESET,	
	"The connection was reset by the remote side."
,WSAECONNABORTED,	
	"The connection was aborted due to timeout or other failure."
,WSAENETDOWN,	
	"The Windows Sockets implementation has detected that "
	"the network subsystem has failed."
,WSANOTINITIALISED,	
	"A successful WSAStartup must occur before using this function."
,WSAENETDOWN,	
	"The Windows Sockets implementation has detected "
	"that the network subsystem has failed."
,WSAEACCES,	
	"The requested address is a broadcast address, but the appropriate flag was not set."
,WSAEINTR,	
	"The (blocking) call was canceled using WSACancelBlockingCall."
,WSAEINPROGRESS,	
	"A blocking Windows Sockets operation is in progress."
,WSAEFAULT,	
	"The buf or to parameters are not part of the user address space, "
	"or the to argument is too small (less than the size of a socket address structure)."
,WSAENETRESET,	
	"The connection must be reset because the Windows Sockets implementation dropped it."
,WSAENOBUFS,	
	"The Windows Sockets implementation reports a buffer deadlock."
,WSAENOTCONN,	
	"The socket is not connected (SOCK_STREAM only)."
,WSAENOTSOCK,	
	"The descriptor is not a socket."
,WSAEOPNOTSUPP,
	"MSG_OOB was specified, but the socket is not of type SOCK_STREAM."
,WSAESHUTDOWN,	
	"The socket has been shut down; it is not possible to sendto on a socket"
	" after shutdown has been invoked with how set to 1 or 2."
,WSAEWOULDBLOCK,	
	"The socket is marked as nonblocking and the requested operation would block."
,WSAEMSGSIZE,	
	"The socket is of type SOCK_DGRAM, and the datagram is larger "
	"than the maximum supported by the Windows Sockets implementation."
,WSAECONNABORTED,	
	"The virtual circuit was aborted due to timeout or other failure."
,WSAECONNRESET,	
	"The virtual circuit was reset by the remote side."
,WSAEADDRNOTAVAIL,	
	"The specified address is not available from the local computer."
,WSAEAFNOSUPPORT,	
	"Addresses in the specified family cannot be used with this socket."
,WSAEDESTADDRREQ,	
	"A destination address is required."
,WSAENETUNREACH,	
	"The network can't be reached from this host at this time."
,WSANOTINITIALISED,	
	"A successful WSAStartup must occur before using this function."
,WSAENETDOWN,	
	"The Windows Sockets implementation has detected that the network"
	" subsystem has failed."
,WSAENOTSOCK,	
	"The descriptor is not a socket."
,WSAEINPROGRESS,	
	"A blocking Windows Sockets call is in progress."
,WSAEINTR,	
	"The (blocking) call was canceled using WSACancelBlockingCall."
,WSAEWOULDBLOCK,	
	"The socket is marked as nonblocking and SO_LINGER is set to a nonzero timeout value."
,WSANOTINITIALISED,	
	"A successful WSAStartup must occur before using this function."
,WSAENETDOWN,	
	"The Windows Sockets implementation has detected that the network"
	" subsystem has failed."
,WSAEINPROGRESS,	
	"A blocking Windows Sockets operation is in progress."
,WSANOTINITIALISED,	
	"A successful WSAStartup must occur before using this function."
,WSAENETDOWN,	
	"The Windows Sockets implementation has detected that the network"
	" subsystem has failed."
,WSAENOTCONN,	
	"The socket is not connected."
,WSAEINTR,	
	"The (blocking) call was canceled using WSACancelBlockingCall."
,WSAEINPROGRESS,	
	"A blocking Windows Sockets operation is in progress."
,WSAENOTSOCK,	
	"The descriptor is not a socket."
,WSAEOPNOTSUPP,	
	"MSG_OOB was specified, but the socket is not of type SOCK_STREAM."
,WSAESHUTDOWN,	
	"The socket has been shut down; it is not possible to recv on a socket"
	" after shutdown has been invoked with how set to 0 or 2."
,WSAEWOULDBLOCK,	
	"The socket is marked as nonblocking and the receive operation would block."
,WSAEMSGSIZE,	
	"The datagram was too large to fit into the specified buffer and was truncated."
,WSAEINVAL,	
	"The socket has not been bound with bind."
,WSAECONNABORTED,	
	"The virtual circuit was aborted due to timeout or other failure."
,WSAECONNRESET,	
	"The virtual circuit was reset by the remote side."
};

void errcom(int errcode){
	
	int	i;
	for(i=0;i<sizeof(errdata)/sizeof(errdata[0]);i++){
		if(errcode==errdata[i].eno){
			::MessageBox(NULL,errdata[i].errmsg,"WINSOCK ERROR !!",MB_OK);
			break;
		}
	}
}

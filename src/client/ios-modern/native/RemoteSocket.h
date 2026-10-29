#pragma once

// A local stream preserves the existing select/recv/send and Protobuf framing.
int OPCreateRemoteSocket();
bool OPConnectRemoteSocket(int descriptor, unsigned long timeoutMilliseconds);
void OPCloseRemoteSocket(int descriptor);

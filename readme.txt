UDP File Transfer
-----------------
Ryan Geary

Uses Go-Back-N protocol based on NAKs: If the client receives a packet with
flipped bits, it will NAK with the sequence number to request the server return
to that position and proceed sending from there.

Relevant files
----------------
- common.{c,h} - code for both client and server
- client.{c,h} - UDP client
- server.{c,h} - UDP server
- makefile     - for compilation

Compilation Instructions
------------------------
1. Run `make`.
2. There is no step 2.

Configuration Files
-------------------
No.

Running Instrucions
-------------------
1. Run `./server <server port number> <window size> <P[Error]>`
  Example: ./server 45000 8 0.1
2. Run `./client <server IP> <server port> <filename>`
  Example: ./client 127.0.0.1 45000 test_file.txt
3. Read the prompts from the client program and act accordingly.

TCP File Transfer
-----------------
Ryan Geary

Relevant files
----------------
- common.{c,h} - code for both client and server
- client.{c,h} - TCP client
- server.{c,h} - TCP server
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
1. Run `./server <server port number>`
  Example: ./server 45000
2. Run `./client <server IP> <server port> <filename>`
  Example: ./client 127.0.0.1 45000 test_file.txt
3. Read the prompts from the client program and act accordingly.

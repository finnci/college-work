SkeletonServer
==============

Ruby implementation of a skeleton server which implements a thread pool.
Currently accepts any number of clients onto the server
Each client will operate on one of 10 worker threads

To Run
======

Type ./start.sh 'portnumber'
To run server on whichever 'portnumber' you choose

Type in "HELO text" 
for client info to be returned to client

Note: literally responds to only the string "HELO text" not HELO followed by some text.


Type in "KILL_SERVICE" 
to kill the server for all clients

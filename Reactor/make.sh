# use source code complie 
g++ -g example.cc eventloop.cc reactor.cc timer.cc -I./ -std=c++11 -o main

# use libeventloop [.a] or [.so] complie
# g++ -g example.cc -I./ -L./ -leventloop -o main
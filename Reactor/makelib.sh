# .so lib
g++ -g eventloop.cc reactor.cc timer.cc -I./ -shared -fPIC -std=c++11 -o libeventloop.so

# .a lib
g++ -c eventloop.cc reactor.cc timer.cc
ar rvs libeventloop.a eventloop.o reactor.o timer.o
rm *.o
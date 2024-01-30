to compile the server file use below command

g++ -o server server.cpp -lboost_system -pthread
OR
g++ -o server server.cpp -I /path of boost -l boost_system -lboost_thread

Steps:
1. Once compiled, execute the build file `./server`
2. This message will be printed on screen `Server is listening on port 8080...`
3. To initate a request use this command form another terminal `telnet 127.0.0.1 8080`
4. you can also use the `client.sh` file to hit 20 request in a loop
5. for consecutive such requests you can run the `client.sh` file in multiple terminal instances.

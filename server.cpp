#include <iostream>
#include <boost/asio.hpp>
#include <vector>
#include <thread>
#include <memory>
#include <csignal>
#include <chrono>

using namespace std;
using namespace boost::asio;

bool serverOff = false;
std::vector<std::thread> activeThreads;

unsigned long long computeFactorial(unsigned int num) {
    if (num == 0 || num == 1) {
        return 1;
    }
    return num * computeFactorial(num - 1);
}

unsigned long long computeSumOfFactorials(unsigned int n) {
    unsigned long long sum = 0;
    for (unsigned int i = 1; i <= n; ++i) {
        sum += computeFactorial(i);
    }
    return sum;
}

void printActiveThreads() {
    cout << "Active threads: " << activeThreads.size() << endl;
}

void handleClient(std::shared_ptr<ip::tcp::socket> socket) {

    // Print Client's information
    ip::tcp::endpoint remoteEndpoint = socket->remote_endpoint();
    cout << "Accepted connection from: " << remoteEndpoint.address().to_string() << ":" << remoteEndpoint.port() << endl;

    // Send Handshake confirmation message to client
    string connectionMessage = "Confirmation from server!\n";
    boost::asio::write(*socket, boost::asio::buffer(connectionMessage));

    // Perform heavy computational task (sum of factorials)
    const unsigned long int targetNumber = rand() % 100000 + 1;

    // Start time measurement
    auto start = std::chrono::steady_clock::now();

    unsigned long long result = computeSumOfFactorials(targetNumber);
    
    // End time measurement
    auto end = std::chrono::steady_clock::now();

    // Calculate the elapsed time in milliseconds
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // if (duration.count() > 10000) { 
    //     serverOff = true;
    // }

    // Send the result back to the client
    string clientMessage = "Sum of factorials up to " + to_string(targetNumber) + ": " + to_string(result);
    std::cout << "Computation took " << duration.count() << " milliseconds." << std::endl;
    boost::asio::write(*socket, boost::asio::buffer(clientMessage));

    printActiveThreads();

    // Detach the thread and remove it from activeThreads
    auto it = find_if(activeThreads.begin(), activeThreads.end(), [&socket](const std::thread& t) {
        return t.get_id() == std::this_thread::get_id();
    });

    if (it != activeThreads.end()) {
        it->detach();  // Detach the thread
        activeThreads.erase(it);  // Remove the thread from the vector
    }
}

void startAcceptor(io_service& ioService, ip::tcp::acceptor& acceptor) {
    auto socket = std::make_shared<ip::tcp::socket>(ioService);

    acceptor.async_accept(*socket, [socket, &ioService, &acceptor](const boost::system::error_code& error) {
        if (!error) {
            // Use std::thread to spawn a new thread for each client
            activeThreads.push_back(std::thread(handleClient, socket));
        }
        // Continue to wait for the next connection
        startAcceptor(ioService, acceptor);
    });
}

int main() {
    try {
        // Creating an I/O service
        io_service ioService;

        // endpoint for listening on port 8080
        ip::tcp::endpoint endpoint(ip::tcp::v4(), 8080);

        // opening an acceptor to listen for incoming connection requests
        ip::tcp::acceptor acceptor(ioService, endpoint);

        cout << "Server is listening on port 8080..." << endl;

        startAcceptor(ioService, acceptor);

        while (!serverOff) {
            ioService.run_one();
        }

        // Join all threads before exiting
        cout << "Joining all threads before exiting";
        for (auto& thread : activeThreads) {
            thread.join();
        }

        std::cout << "Server shut down." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION" << e.what() << '\n';
    }
    return 0;
}

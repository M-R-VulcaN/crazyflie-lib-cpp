#pragma once

#include <queue>
#include <list>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include "crazyflieLinkCpp/Connection.h"
#include "crazyflieLinkCpp/Packet.hpp"
#include <functional>
#include <atomic>
// #include <boost/atomic/



//The PacketCallback returns a boolean,
//If it returns true then the callback will be called for another loop
//If it returns false then the callback is removed from the list
typedef std::function<bool(bitcraze::crazyflieLinkCpp::Packet)> PacketCallback;

//Includes the port and channel of the callback
struct PacketCallbackBundle
{
    uint8_t _port;
    uint8_t _channel;
    PacketCallback _packetCallbackFunc;
};

//This class is the main worker class that is responsible for sending callbacks
// on received packets from the crazyflie server
//It is also a thin rapper around the Connection class 
class ConnectionWorker
{
private:
    std::list<PacketCallbackBundle> _paramReceivedCallbacks;//The list of callbacks that are present
    std::thread _receivingThread;//The mutex for receiving packets
    std::mutex _threadSleepMutex; //The mutex for changing the receive-thread's state from active to inactive or vice versa
    std::mutex _callbackMutex; // The mutex for running a callback function
    std::shared_timed_mutex _callbackSharedMutex;

    std::condition_variable _threadSleepConVar; //Condition variable which is used for waiting whenever the receive-thread is sleeping
    bitcraze::crazyflieLinkCpp::Connection * _conAtomicPtr; // An atomic pointer to the Connection object so that you could do multi-thread activities on it
    std::atomic<bool> _deactivateThread; //a switch for activating or deactivated the thread
    void receivePacketsThreadFunc();// The function which runs on the packets receive thread

public:
    //Constructs the connection worker, takes a Connection reference as a parameter to store a pointer to it
    //The thread is active when the constuction is finished however it is still sleeping
    ConnectionWorker(bitcraze::crazyflieLinkCpp::Connection &con);

    //Destructor for the connection worker
    //This stops the thread completely 
    ~ConnectionWorker();

    //start wakes up the thread from its sleeping
    //This method needs to be used after the constructor or after stop() is called
    //To make it work again
    void start();

    //Puts the thread back into sleep
    void stop();
    
    //appends a callback to the callback list
    //The callback will be removed from the callback list whenever it returns false
    void addCallback(const PacketCallbackBundle &callback);

    //sends a packet to the crazyflie
    void send(const bitcraze::crazyflieLinkCpp::Packet &p);
};

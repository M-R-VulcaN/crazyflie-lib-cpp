#include "ConnectionWorker.h"
#include <iostream>
#include <chrono>

using bitcraze::crazyflieLinkCpp::Connection;
using bitcraze::crazyflieLinkCpp::Packet;


ConnectionWorker::ConnectionWorker(Connection &con) : _conAtomicPtr(&con)
{
    _receivingThread = std::thread(&ConnectionWorker::receivePacketsThreadFunc, this);
    _deactivateThread = true;
    _receivingThread.detach();
}

ConnectionWorker::~ConnectionWorker()
{
    _conAtomicPtr = nullptr;
    this->stop();
}

void ConnectionWorker::start()
{
    std::lock_guard<std::mutex> lock(_threadSleepMutex);
    _deactivateThread = false;

    _threadSleepConVar.notify_all();
}
void ConnectionWorker::stop()
{
    std::lock_guard<std::mutex> lock(_threadSleepMutex);
    _deactivateThread = true;

    _threadSleepConVar.notify_all();
}
void ConnectionWorker::addCallback(const PacketCallbackBundle &callback)
{
    std::lock_guard<std::mutex> lock(_callbackMutex);
    _paramReceivedCallbacks.push_back(callback);
}

void ConnectionWorker::receivePacketsThreadFunc()
{
    Packet p_recv;
    while (true)
    {
 
        if (_deactivateThread)
        {
            std::atomic<bool>* deactivateThreadPtr = &_deactivateThread;
            std::unique_lock<std::mutex> lock(_threadSleepMutex);
            _threadSleepConVar.wait(lock, [deactivateThreadPtr](){return !*deactivateThreadPtr;});
        }


        if (_conAtomicPtr)
        {
            p_recv = ((Connection *)_conAtomicPtr)->recv(1);
        }
        else
            break;

        if (p_recv && !_deactivateThread)
        {

            // if(p_recv.port() == 5 && p_recv.channel()==2) //debug
                // std::cout << p_recv <<std::endl;
            auto* paramReceivedCallbacksPtr = &_paramReceivedCallbacks;
            auto* p_recvPtr=&p_recv;
            auto* callbackMutexPtr = &_callbackMutex;
            std::condition_variable waitForIterator;
            std::condition_variable* waitForIteratorPtr = &waitForIterator;
            std::mutex mu;
            std::mutex* muPtr = &mu;
            std::unique_lock<std::mutex> waitForIteratorLock(mu);
            std::lock_guard<std::mutex> lock(_callbackMutex);
            std::atomic<bool> isIteratorFinished(false);
            std::atomic<bool>* isIteratorFinishedPtr = &isIteratorFinished;

            for(auto it = _paramReceivedCallbacks.begin(); it != _paramReceivedCallbacks.end(); it++)
            {
                auto* itPtr = &it;
                if(p_recv.channel() == it->_channel && it->_port == p_recv.port())
                {
                    std::thread thread([paramReceivedCallbacksPtr, itPtr,p_recvPtr, callbackMutexPtr,muPtr,waitForIteratorPtr,isIteratorFinishedPtr](){
                        std::unique_lock<std::mutex> lock(*muPtr);
                        auto it = *itPtr;
                        lock.unlock();
                        *isIteratorFinishedPtr = true;
                        waitForIteratorPtr->notify_all();
                        if(!it->_packetCallbackFunc(*p_recvPtr))
                        {
                            std::lock_guard<std::mutex> lock(*callbackMutexPtr);
                            paramReceivedCallbacksPtr->erase(it);
                        }
                    });
                    thread.detach();
                    waitForIterator.wait(waitForIteratorLock,[isIteratorFinishedPtr](){return (bool)*isIteratorFinishedPtr;});
                }
            }
          
        }
    }
}
void ConnectionWorker::send(const Packet &p)
{
    ((Connection *)_conAtomicPtr)->send(p);
}

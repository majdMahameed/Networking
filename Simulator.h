

#ifndef Simulator_hpp
#define Simulator_hpp

#include <stdio.h>
#include <queue>
#include <random>
#include <vector>

#include "Event.h"

using std::vector;
using std::queue;
using std::shared_ptr;
using randomEngine = std::default_random_engine;
using expDist = std::exponential_distribution<double>;

class Server;
class Simulator;

class PacketArrivalEvent : public Event {
    Server* server;
public:
    PacketArrivalEvent(double arrival, Server* server);
    void process() const override;
};

class PacketDepartureEvent : public Event {
    public:
    const double packetArrivalTime;
    const double serviceTime;
    Server* server;

    PacketDepartureEvent(double arrivalTime, double departureTime, double serviceTime, Server* server);
    void process() const override;
};

class Server {
    public:
    queue<shared_ptr<PacketArrivalEvent>> packets;
    const unsigned int server_size;
    const double service_rate;
    randomEngine generator;
    expDist serviceTime;
    bool busy;
    Simulator& simulator;

    Server(Simulator& sim, unsigned int maxSize, double serviceRate);
    ~Server() = default;

    void ArrivePacket(const PacketArrivalEvent& packet);
    void ServicePacket(const PacketDepartureEvent& packet);
};



class Simulator {
public:
    std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator> events;
    int processed_frames = 0;
    int tossed_frames = 0;
    double total_wait_time = 0;
    double total_service_time = 0;
    double total_time;
    double current_time = 0;
    vector<Server> servers;
    vector<double> probabilities;


    Server* SelectServer();
    Simulator(double simTime, const vector<int>& serversSize,
                    const vector<double>& probabilityPerServer, double arrivalRate,
                    const vector<double>& serviceRatePerServer);
    Simulator(const Simulator&) = delete;
    Simulator& operator=(const Simulator&) = delete;
    ~Simulator() = default;

    void scheduleEvent(std::shared_ptr<Event> event);
    void run();
    void print() const;

};

#endif /* Simulator_hpp */
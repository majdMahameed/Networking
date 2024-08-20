

#include "Simulator.h"

using std::make_shared;

PacketArrivalEvent::PacketArrivalEvent(double arrival, Server* server) : Event(arrival), server(server) {}

void PacketArrivalEvent::process() const {
    server->ArrivePacket(*this);
}

PacketDepartureEvent::PacketDepartureEvent(double arrivalTime, double departureTime, double serviceTime, Server* server) : Event(departureTime), packetArrivalTime(arrivalTime), serviceTime(serviceTime), server(server) {}



void PacketDepartureEvent::process() const  {
    server->ServicePacket(*this);
}


Server::Server(Simulator& sim, unsigned int maxSize, double serviceRate) : server_size(maxSize), service_rate(serviceRate), serviceTime(service_rate), busy(false), simulator(sim) {
    std::random_device rd;  // Create a random device
    unsigned int seed = rd();  // Generate a seed
    generator.seed((unsigned int) seed);
}


void Server::ArrivePacket(const PacketArrivalEvent& packet){
    if(this->busy == false)
    {
        busy = true;
        double arrivalTime = packet.getEventTime();
        double servingTime = serviceTime(generator);
        double departureTime = arrivalTime + servingTime;
        std::shared_ptr<Event> newEvent = make_shared<PacketDepartureEvent>(arrivalTime, departureTime, servingTime, this);
        simulator.scheduleEvent((newEvent));
    }
    else
    {
        if(this->packets.size() >= this->server_size){
            simulator.tossed_frames++;
        }
        else
            packets.push(make_shared<PacketArrivalEvent>(packet));
    }
}

void Server::ServicePacket(const PacketDepartureEvent& packet)
{
    simulator.processed_frames++;
    simulator.total_wait_time += packet.getEventTime() - packet.serviceTime - packet.packetArrivalTime;
    simulator.total_service_time += packet.serviceTime;
    if(packets.empty())
    {
        busy = false;
        return;
    }
    shared_ptr<PacketArrivalEvent> toServe = packets.front();
    double time = toServe->getEventTime();
    double servingTime = serviceTime(generator);
    double departureTime = packet.getEventTime() + servingTime;
    std::shared_ptr<Event> newEvent = make_shared<PacketDepartureEvent>(time, departureTime, servingTime, this);
    simulator.scheduleEvent(newEvent);
    packets.pop();

}

Server* Simulator::SelectServer() {
    // Generate a random number
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    double random_number = dis(gen);

    // Select an element
    for (size_t i = 0; i < servers.size(); ++i) {
        if (random_number < probabilities[i]) {
            return &servers[i];
        }
        random_number -= probabilities[i];
    }
    return nullptr;
}

Simulator::Simulator(double simTime, const vector<int>& serversSizes,const vector<double>& probabilityPerServer, double arrivalRate,const vector<double>& serviceRatePerServer):total_time(simTime),
probabilities(probabilityPerServer)
{
    expDist arrivalDistribution(arrivalRate);
    randomEngine generator;
    std::random_device rd;  // Create a random device
    unsigned int seed = rd();  // Generate a seed
    generator.seed((unsigned int) seed);

    for(int i=0; i< serversSizes.size(); i++)
        servers.push_back(Server(*this, serversSizes[i], serviceRatePerServer[i]));

    double time = 0;
    std::shared_ptr<Event> newEvent = make_shared<PacketArrivalEvent>(time, SelectServer());
    scheduleEvent(newEvent);
    while(time < simTime){
        time += arrivalDistribution(generator);
        newEvent = make_shared<PacketArrivalEvent>(time, SelectServer());
        scheduleEvent(newEvent);
    }
}
void Simulator::scheduleEvent(std::shared_ptr<Event> event){
    events.push(event);
}

void Simulator::run(){
    while(events.size() != 0 && current_time < total_time)
    {
        const auto& event = events.top();
        current_time = event->getEventTime();
        event->process();
        events.pop();
    }
}

void Simulator::print() const {
    std::cout << this->processed_frames << " " << this->tossed_frames << " ";
    std::cout << this->current_time << " " <<  total_wait_time / this->processed_frames << " " << total_service_time / this->processed_frames;
    return;
}



int main(int argc, const char * argv[]) {
    // parse input from command line
    double totalSimTime = std::stod(argv[1]);
    int serversNum = std::stoi(argv[2]);
    double poisson_rate = std::stod(argv[3+serversNum]);

    std::vector<double> probabilities;
    for(int i = 0; i < serversNum; i++) {
        probabilities.push_back(std::stod(argv[3+i]));
    }
    std::vector<int> serversQueueSizes;
    for(int i = 0; i < serversNum; i++) {
        serversQueueSizes.push_back(std::stod(argv[4+serversNum+i]));
    }
    std::vector<double> serviceRate;
    for(int i = 0; i < serversNum; i++) {
        serviceRate.push_back(std::stod(argv[4+2*serversNum+i]));
    }

    // run simulation
    Simulator simulator(totalSimTime, serversQueueSizes, probabilities, poisson_rate, serviceRate);
    simulator.run();
    simulator.print();
    return 0;
}

#include <iostream>
#include <vector>
#include <random>




#define ARRIVAL 1
#define SERVICE 0

template<typename Frame>
class Server {
    public:
    int max_size;
    std::vector<Frame>* queue = new std::vector<Frame>();
    double probability;
    double service_rate;
    double last_service_time;
    Server(int size, double P, double service_rate): max_size(size),probability(P),
    service_rate(service_rate){}
    ~Server() = default;
};


template<typename Frame,typename Server>
class Simulator{
    public:

    double run_time = 0;
    int processed_count = 0;
    int tossed_count = 0;
    double wait_time = 0;
    double service_time = 0;

    double total_time;
    int servers_num;

    std::vector<Frame>* frame_arrival_queue;
    std::vector<Server>* servers_queue;

    Simulator(double T, int N, std::vector<Frame>* frame_arrival_queue, std::vector<Server>* servers_queue)
              :total_time(T), servers_num(N), frame_arrival_queue(frame_arrival_queue), servers_queue(servers_queue){}

    ~Simulator() = default;

    void run(){
        Frame* current_frame = &frame_arrival_queue->at(0);
        double time = 0;
        Server* server = servers_queue->data();
        bool is_process = false;
        while (current_frame != nullptr && total_time >= run_time){
            current_frame = &frame_arrival_queue->at(0);
            int j = 0;
            while (j < servers_num){
                if ( server[j].queue->empty() == false){
                    if (current_frame->full_process_time > server[j].queue[0].data()->full_process_time){
                        current_frame = server[j].queue[0].data();
                        is_process = true;
                    }
                }
                j++;
            }
            if (is_process == true){
                current_frame->process(SERVICE);
                is_process = false;
            } else{
                current_frame->process(ARRIVAL);
                this->frame_arrival_queue->erase(this->frame_arrival_queue->begin());
            }
        }
        Print();
    }
    void Print(){
        std::cout << this->processed_count << " " << this->tossed_count << " " << this->run_time
        << " " << this->wait_time/ this->processed_count << " " << this->service_time/ this->processed_count;
    }
};
class Frame{
public:

    double full_process_time = 0;
    //arrival time contain arrival time from the beginning
    double arrival_time;
    int server_num;
    double wait_time = 0;
    double service_time = 0;
    Simulator<Frame,Server<Frame>>* simulator;

    Frame(double arrival_time, int server_num, double service_time, Simulator<Frame,Server<Frame>>* simulator){
        this->arrival_time = arrival_time;
        this->full_process_time = arrival_time;
        this->server_num = server_num;
        this->simulator = simulator;
    }
    ~Frame() = default;
    void process(int type_process = 1){
        Server<Frame>* server = simulator->servers_queue->data();
        if (type_process == ARRIVAL){
            simulator->run_time = this->arrival_time;
            if (server[server_num].queue->size() == server[server_num].max_size ){
                simulator->tossed_count++;
            }else if (server[server_num].queue->empty()){
                std::mt19937 rd;
                double rand = rd();
                this->service_time = (log(rand))/ server[server_num].service_rate;
                this->full_process_time = this->service_time + this->arrival_time;
                server->last_service_time = this->full_process_time;
                server[server_num].queue->push_back(*this);
            }else{
                std::mt19937 rd;
                double rand = rd();
                this->service_time = (log(rand))/ server[server_num].service_rate;
                this->full_process_time = this->service_time + this->arrival_time;
                server[server_num].queue->push_back(*this);
            }
        } else if (type_process == SERVICE){
            simulator->processed_count++;
            simulator->wait_time += this->wait_time;
            std::vector<Frame>* queue = server[server_num].queue;
            Frame* frame = queue->data();
            int i = 1;
            while (i < queue->size()){
                frame[i].wait_time += this->service_time;
                i++;
            }
            simulator->service_time += this->service_time;
            simulator->run_time = this->full_process_time;
            server[server_num].queue->erase(queue->begin());
        }
    };

};


int main(int argc, char* argv[]) {

  /*  int num_of_servers = int(*argv[1]);
    double total_time = double (*argv[0]);
    int num_of_frames = total_time * int (*argv[num_of_servers + 2]);
    int poisson_rate = double (*argv[num_of_servers + 2]);
*/
    int num_of_servers = 2;
    double total_time = 0.5;
    int num_of_frames = total_time * 200;
    int poisson_rate = 200;

    std::vector<Frame>* frames_arrival_queue = new std::vector<Frame>();
    std::vector<Server<Frame>>* servers_queue = new std::vector<Server<Frame>>();
    std::vector<double> probabilities = { 0.2 , 0.8};
    std::vector<double> service_rate_queue = { 20 , 190};
    std::vector<double> server_size = { 2 , 10};

    std::uniform_real_distribution<> dis(0.0, 1.0);

    Simulator<Frame,Server<Frame>>* simulator = new Simulator<Frame,Server<Frame>>(total_time, num_of_servers,frames_arrival_queue,servers_queue);
    double time = 0;
    //Assign properties to servers
    for (int i = 0; i < num_of_servers; ++i) {
      //  probabilities.push_back(double(*argv[i + 2]));
      //  Server<Frame>* server = new Server<Frame>(int(*argv[i + 3 + num_of_servers]),probabilities.back(),int(*argv[i + 3 + 2*num_of_servers]));
        Server<Frame>* server = new Server<Frame>(server_size[i],probabilities[i],service_rate_queue[i]);
        servers_queue->push_back(*server);
    }
    // Check that probabilities sum to 1
    double sum = 0.0;
    for (double p : probabilities) {
        sum += p;
    }
    if (sum != 1.0) {
        return 0;
    }
    //Generate Arrival times for the packets
    for (int i = 0; i < num_of_frames; ++i) {
        std::mt19937 rd;
        double rand = rd();
        double arrival = log(rand)/ poisson_rate;
        time += arrival;
        std::random_device gd;  // Seed for the random number engine
        std::mt19937 gen(gd()); // Mersenne Twister random number engine

        // Define a distribution that produces values between 0 and 1
        std::uniform_real_distribution<> dis(0.0, 1.0);

        // Generate a random number between 0 and 1
        double randomValue = dis(gen);      //  double randomValue = dis(rd);  // Generate a random number between 0 and 1                                                                                                              ס
        double cumulativeProbability = 0.0;
        int server_num = 0;
        // Determine which server receives the packet
        for (int j = 0; j < num_of_servers; ++j) {
            cumulativeProbability += probabilities[j];
            if (randomValue <= cumulativeProbability) {
                server_num = j;
                break;
            }
        }
        Frame* frame = new Frame(time,server_num,0,simulator);
        frames_arrival_queue->push_back(*frame);
    }
    simulator->run();
    return 0;
}

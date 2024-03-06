#include "icast.hpp"

void Icast::init(std::string config_path, bool print_structure)
{
    if (config_path == "") {
        char* icast_cfg_path = getenv("ICAST_DIR");

        config_path = icast_cfg_path;
    }

    // Load configuration on .cfg
    std::ifstream file(config_path + "/icast.cfg");

    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return;
    }

    int agent;
    std::string dictionary_path;
    std::string multicast_ip;
    int multicast_port;
    std::string multicast_interface;
    int multicast_period_ms;

    std::string line;

    while (std::getline(file, line)) {
        if (line[0] == '#') {
            continue;
        }

        // erase all spaces
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        std::istringstream is_line(line);
        std::string key;

        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                if (key == "agent") {
                    agent = std::stoi(value);
                } else if (key == "dictionary_path") {
                    dictionary_path = config_path + "/" + value;
                } else if (key == "multicast_ip") {
                    multicast_ip = value;
                } else if (key == "multicast_port") {
                    multicast_port = std::stoi(value);
                } else if (key == "multicast_interface") {
                    multicast_interface = value;
                } else if (key == "multicast_period_ms") {
                    multicast_period_ms = std::stoi(value);
                }
            }
        }
    }

    dc->init(agent, dictionary_path);
    if (print_structure) {
        dc->structureDisplay();
    }

    mc->init(multicast_ip, multicast_port, multicast_interface, multicast_period_ms, MC_LOOPBACK);
    if (!mc->initialized()) {
        std::cout << "Multicast not ready" << std::endl;
        return;
    }
}

void Icast::update()
{
    mc->tick = mc->millis();

    if (mc->readyToSend()) {
        std::vector<uint8_t> selected_data;
        dc->packetProcessTransmit(selected_data);
        if (selected_data.size() > 0) {
            mc->send(selected_data);
        }
    }

    std::vector<uint8_t> received_data;
    mc->recv(received_data);
    if (received_data.size() > 0) {
        dc->packetProcessReceive(received_data);
    }

    memcpy((void*)&dc->data_bus, &dc->dictionary_data_[0], sizeof(icast_bus_t));
}
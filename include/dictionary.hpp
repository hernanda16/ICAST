#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <fstream>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "iomanip"

typedef struct
{
    std::string name;
    size_t offset;
    size_t size;
    std::string type;
} DictLevel3;
typedef struct
{
    std::string name;
    size_t offset;
    size_t size;
    std::vector<DictLevel3> values;
    bool isUpdatedLocal;
    bool isUpdatedRemote;
} DictLevel2;
typedef struct
{
    std::string name;
    size_t offset;
    size_t size;
    std::vector<DictLevel2> fields;
} DictLevel1;

class Dictionary {
private:
    Dictionary() { }

public:
    static Dictionary* getInstance()
    {
        static Dictionary instance_;
        return &instance_;
    }

    void operator=(const Dictionary&) = delete;
    Dictionary(Dictionary& other) = delete;

    //Parameter
    //=========
    int whoami_;
    std::string path_to_dictionary_;

    // Dictionary
    // ==========
    std::vector<DictLevel1> dictionary_structure_;
    std::vector<uint8_t> dictionary_data_;

    /**
     * @brief Initialize dictionary based on dictionary.json
     *
     * @param whoami The ID of the agent
     * @param path_to_dictionary The path to the dictionary.json file
     */
    void init(int whoami, std::string path_to_dictionary);

    /**
     * @brief Display dictionary structure
     *
     */
    void structureDisplay();

    /**
     * @brief Display dictionary data
     *
     */
    void dataDisplay();

    /**
     * @brief Get a string of the value inside the dictionary data
     *
     * @param offset Offset of the value in the dictionary data
     * @param size Size of the value in the dictionary data
     * @param type Type of the value
     * @return std::string String of the value
     */
    std::string getValueString(const size_t offset, const size_t size, const std::string type);

    /**
     * @brief Get the offset and size of a key in the dictionary structure
     *
     * @param id The ID of the agent
     * @param key The key to search for in the dictionary structure
     * @param offset Return offset of the key in the dictionary data
     * @param size Return size of the key in the dictionary data
     */
    void getOffsetSize(const int id, const std::string key, size_t& offset, size_t& size);

    /**
     * @brief Set or reset the isUpdatedLocal or isUpdatedRemote flag for a given key in the dictionary structure
     *
     * @param id The ID of the agent
     * @param key The key to search for in the dictionary structure
     * @param localremote A boolean flag indicating whether to set/reset the isUpdatedLocal or isUpdatedRemote flag
     * @param setreset A boolean flag indicating whether to set or reset the flag
     */
    void setResetUpdate(const int id, const std::string key, const bool localremote, const bool setreset);

    /**
     * @brief Add data to the transmit buffer
     *
     * @param packet The transmit buffer
     * @param offset Offset of the data in the dictionary data
     * @param size Size of the data
     */
    void packetAdd(std::vector<uint8_t>& packet, const size_t offset, const size_t size);

    /**
     * @brief Generate transmit packet from dictionary data and structure
     *
     * @param packet The transmit packet
     */
    void packetProcessTransmit(std::vector<uint8_t>& packet);

    /**
     * @brief Process received packet and update dictionary data and structure
     *
     * @param packet The received packet
     */
    void packetProcessReceive(const std::vector<uint8_t> packet);
};

#endif
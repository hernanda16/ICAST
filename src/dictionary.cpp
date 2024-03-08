#include "dictionary.hpp"

void Dictionary::init(int whoami, std::string path_to_dictionary)
{
    whoami_ = whoami;
    path_to_dictionary_ = path_to_dictionary;
    // Initialize an empty dictionary JSON object
    std::string dictionary_string = "{}";
    rapidjson::Document dictionary_json = rapidjson::Document();
    dictionary_json.SetObject();

    // Attempt to open the dictionary file
    std::fstream fs(path_to_dictionary_, std::ios::in);
    if (!fs.is_open())
    {
        // If the file does not exist, create it and write an empty dictionary to it
        std::fstream fs(path_to_dictionary_, std::ios::out);
        fs << "{}";
        fs.close();
    }
    else
    {
        // If the file exists, read its contents into the dictionary string
        dictionary_string = std::string((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
        fs.close();
    }

    // Parse the dictionary string into a JSON object
    dictionary_json.Parse(dictionary_string.c_str());
    if (dictionary_json.HasParseError())
    {
        // If the parse fails, initialize an empty dictionary
        dictionary_string = "{}";
        dictionary_json = rapidjson::Document();
        dictionary_json.SetObject();
    }

    // Iterate over the levels of the dictionary JSON object
    for (auto &level1 : dictionary_json.GetObject())
    {
        // Initialize a new level 1 dictionary object
        DictLevel1 dict_level1;
        dict_level1.name = level1.name.GetString();
        dict_level1.offset = dictionary_data_.size();
        dict_level1.size = 0;

        // Iterate over the fields of the level 1 dictionary object
        for (auto &level2 : level1.value.GetObject())
        {
            // Initialize a new level 2 dictionary object
            DictLevel2 dict_level2;
            dict_level2.name = level2.name.GetString();
            dict_level2.offset = dictionary_data_.size();
            dict_level2.size = 0;
            dict_level2.isUpdatedLocal = false;
            dict_level2.isUpdatedRemote = false;

            // Iterate over the values of the level 2 dictionary object
            for (auto &level3 : level2.value.GetObject())
            {
                // Initialize a new level 3 dictionary object
                DictLevel3 dict_level3;
                dict_level3.name = level3.name.GetString();
                dict_level3.offset = dictionary_data_.size();
                dict_level3.type = level3.value.GetString();

                // Extract the size of the value from the type string
                dict_level3.size = 0;
                for (char c : dict_level3.type.substr(0, dict_level3.type.find("[")))
                {
                    if (isdigit(c))
                    {
                        dict_level3.size = dict_level3.size * 10 + (c - '0');
                    }
                }

                // Type string may contain "[xxx]" which means the value is an array of xxx elements
                if (dict_level3.type.find("[") != std::string::npos && dict_level3.type.find("]") != std::string::npos)
                {
                    unsigned int n_elements = std::stoi(dict_level3.type.substr(dict_level3.type.find("[") + 1, dict_level3.type.find("]") - dict_level3.type.find("[") - 1));
                    dict_level3.size *= n_elements;
                }

                // Resize the dictionary data vector to accommodate the value
                dictionary_data_.resize(dictionary_data_.size() + dict_level3.size);

                // Add the level 3 dictionary object to the level 2 dictionary object
                dict_level2.size += dict_level3.size;
                dict_level2.values.push_back(dict_level3);
            }

            // Add the level 2 dictionary object to the level 1 dictionary object
            dict_level1.size += dict_level2.size;
            dict_level1.fields.push_back(dict_level2);
        }

        // Add the level 1 dictionary object to the dictionary structure vector
        dictionary_structure_.push_back(dict_level1);
    }

    // Initialize the dictionary data vector with zeros
    dictionary_data_.assign(dictionary_data_.size(), 0);
}

void Dictionary::structureDisplay()
{
    printf("+----------------------+--------+------+-----------------+\n");
    printf("| Name                 | Offset | Size | Type            |\n");
    printf("+----------------------+--------+------+-----------------+\n");
    for (auto &level1 : dictionary_structure_)
    {
        printf("| %-20s |        |      |                 |\n", level1.name.c_str());
        for (auto &level2 : level1.fields)
        {
            bool is_last_level2 = &level2 == &level1.fields.back();
            if (is_last_level2)
            {
                printf("| └── %-16s | %6ld | %4ld |                 |\n", level2.name.c_str(), level2.offset, level2.size);
            }
            else
            {
                printf("| ├── %-16s | %6ld | %4ld |                 |\n", level2.name.c_str(), level2.offset, level2.size);
            }
            for (auto &level3 : level2.values)
            {
                bool is_last_level3 = &level3 == &level2.values.back();
                if (is_last_level2 && is_last_level3)
                    printf("|     └── %-12s | %6ld | %4ld | %15s |\n", level3.name.c_str(), level3.offset, level3.size, level3.type.c_str());
                else if (is_last_level2 && !is_last_level3)
                    printf("|     ├── %-12s | %6ld | %4ld | %15s |\n", level3.name.c_str(), level3.offset, level3.size, level3.type.c_str());
                else if (!is_last_level2 && is_last_level3)
                    printf("| │   └── %-12s | %6ld | %4ld | %15s |\n", level3.name.c_str(), level3.offset, level3.size, level3.type.c_str());
                else if (!is_last_level2 && !is_last_level3)
                    printf("| │   ├── %-12s | %6ld | %4ld | %15s |\n", level3.name.c_str(), level3.offset, level3.size, level3.type.c_str());
            }
        }
    }
    printf("+----------------------+--------+------+-----------------+\n");
}

void Dictionary::dataDisplay()
{
    printf("+-------------------------+-----------------+\n");
    printf("| Name                    | Value           |\n");
    printf("+-------------------------+-----------------+\n");
    for (auto &level1 : dictionary_structure_)
    {
        printf("| %-23s |                 |\n", level1.name.c_str());
        for (auto &level2 : level1.fields)
        {
            bool is_last_level2 = &level2 == &level1.fields.back();
            if (is_last_level2)
            {
                printf("| └── %-19s |                 |\n", level2.name.c_str());
            }
            else
            {
                printf("| ├── %-19s |                 |\n", level2.name.c_str());
            }
            for (auto &level3 : level2.values)
            {
                bool is_last_level3 = &level3 == &level2.values.back();
                if (is_last_level2 && is_last_level3)
                    printf("|     └── %-15s | %-15s |\n", level3.name.c_str(), getValueString(level3.offset, level3.size, level3.type).c_str());
                else if (is_last_level2 && !is_last_level3)
                    printf("|     ├── %-15s | %-15s |\n", level3.name.c_str(), getValueString(level3.offset, level3.size, level3.type).c_str());
                else if (!is_last_level2 && is_last_level3)
                    printf("| │   └── %-15s | %-15s |\n", level3.name.c_str(), getValueString(level3.offset, level3.size, level3.type).c_str());
                else if (!is_last_level2 && !is_last_level3)
                    printf("| │   ├── %-15s | %-15s |\n", level3.name.c_str(), getValueString(level3.offset, level3.size, level3.type).c_str());
            }
        }
    }
    printf("+-------------------------+-----------------+\n");
}

std::string Dictionary::getValueString(const size_t offset, const size_t size, const std::string type)
{
    std::string value;

    // If type contains 'float' and size is 4, the value is a float
    if (type.find("float") != std::string::npos && size == 4)
    {
        float value_float;
        std::copy(dictionary_data_.begin() + offset, dictionary_data_.begin() + offset + size, (uint8_t *)&value_float);
        value = std::to_string(value_float);
    }
    // If type contains 'float' and size is 8, the value is a double
    else if (type.find("float") != std::string::npos && size == 8)
    {
        double value_double;
        std::copy(dictionary_data_.begin() + offset, dictionary_data_.begin() + offset + size, (uint8_t *)&value_double);
        value = std::to_string(value_double);
    }
    // If type contains 'int' and size is 1, the value is an int8
    else if (type.find("int") != std::string::npos && size == 1)
    {
        int8_t value_int8;
        std::copy(dictionary_data_.begin() + offset, dictionary_data_.begin() + offset + size, (uint8_t *)&value_int8);
        value = std::to_string(value_int8);
    }
    // If type contains 'int' and size is 2, the value is an int16
    else if (type.find("int") != std::string::npos && size == 2)
    {
        int16_t value_int16;
        std::copy(dictionary_data_.begin() + offset, dictionary_data_.begin() + offset + size, (uint8_t *)&value_int16);
        value = std::to_string(value_int16);
    }
    // If type contains 'int' and size is 4, the value is an int32
    else if (type.find("int") != std::string::npos && size == 4)
    {
        int32_t value_int32;
        std::copy(dictionary_data_.begin() + offset, dictionary_data_.begin() + offset + size, (uint8_t *)&value_int32);
        value = std::to_string(value_int32);
    }
    // If type contains 'int' and size is 8, the value is an int64
    else if (type.find("int") != std::string::npos && size == 8)
    {
        int64_t value_int64;
        std::copy(dictionary_data_.begin() + offset, dictionary_data_.begin() + offset + size, (uint8_t *)&value_int64);
        value = std::to_string(value_int64);
    }
    // If type contains 'uint' and size is 1, the value is an uint8
    else if (type.find("uint") != std::string::npos && size == 1)
    {
        uint8_t value_uint8;
        std::copy(dictionary_data_.begin() + offset, dictionary_data_.begin() + offset + size, (uint8_t *)&value_uint8);
        value = std::to_string(value_uint8);
    }
    // If type contains 'uint' and size is 2, the value is an uint16
    else if (type.find("uint") != std::string::npos && size == 2)
    {
        uint16_t value_uint16;
        std::copy(dictionary_data_.begin() + offset, dictionary_data_.begin() + offset + size, (uint8_t *)&value_uint16);
        value = std::to_string(value_uint16);
    }
    // If type contains 'uint' and size is 4, the value is an uint32
    else if (type.find("uint") != std::string::npos && size == 4)
    {
        uint32_t value_uint32;
        std::copy(dictionary_data_.begin() + offset, dictionary_data_.begin() + offset + size, (uint8_t *)&value_uint32);
        value = std::to_string(value_uint32);
    }
    // If type contains 'uint' and size is 8, the value is an uint64
    else if (type.find("uint") != std::string::npos && size == 8)
    {
        uint64_t value_uint64;
        std::copy(dictionary_data_.begin() + offset, dictionary_data_.begin() + offset + size, (uint8_t *)&value_uint64);
        value = std::to_string(value_uint64);
    }
    // If type contains 'string', the value is a string
    else if (type.find("string") != std::string::npos)
    {
        value = std::string(dictionary_data_.begin() + offset, dictionary_data_.begin() + offset + size);
    }

    return value;
}

void Dictionary::getOffsetSize(const int id, const std::string key, size_t &offset, size_t &size)
{
    std::string key_level1 = "agent" + std::to_string(id);
    std::string key_level2;
    std::string key_level3;

    // If the key is empty, the key is at level 1
    if (key == "")
    {
        for (auto level1 : dictionary_structure_)
        {
            if (level1.name == key_level1)
            {
                offset = level1.offset;
                size = level1.size;
                return;
            }
        }
    }

    // Count the number of slashes in the key to determine the level of the key
    int slash_count = 0;
    for (char c : key)
    {
        if (c == '/')
        {
            slash_count++;
        }
    }

    // If there are no slashes, the key is at level 2
    if (slash_count == 0)
    {
        key_level2 = key;
    }
    // If there is one slash, the key is at level 3
    else if (slash_count == 1)
    {
        key_level2 = key.substr(0, key.find_first_of("/"));
        key_level3 = key.substr(key.find_first_of("/") + 1);
    }

    // Search for the key in the dictionary structure
    if (slash_count == 0)
    {
        for (auto level1 : dictionary_structure_)
        {
            if (level1.name == key_level1)
            {
                for (auto level2 : level1.fields)
                {
                    if (level2.name == key_level2)
                    {
                        offset = level2.offset;
                        size = level2.size;
                        return;
                    }
                }
            }
        }
    }
    else if (slash_count == 1)
    {
        for (auto level1 : dictionary_structure_)
        {
            if (level1.name == key_level1)
            {
                for (auto level2 : level1.fields)
                {
                    if (level2.name == key_level2)
                    {
                        for (auto level3 : level2.values)
                        {
                            if (level3.name == key_level3)
                            {
                                offset = level3.offset;
                                size = level3.size;
                                return;
                            }
                        }
                    }
                }
            }
        }
    }

    // If the key is not found, set the offset and size to 0
    offset = size = 0;
}

void Dictionary::setResetUpdate(const int id, const std::string key, const bool localremote, const bool setreset)
{
    std::string key_level1 = "agent" + std::to_string(id);
    std::string key_level2;

    // If the key is empty, set/reset the isUpdatedLocal or isUpdatedRemote flag for all fields at level 2
    if (key == "")
    {
        for (auto &Level1 : dictionary_structure_)
        {
            if (Level1.name == key_level1)
            {
                for (auto &level2 : Level1.fields)
                {
                    if (!localremote)
                    {
                        // Set/reset the isUpdatedLocal flag for the field
                        level2.isUpdatedLocal = setreset;
                    }
                    else
                    {
                        // Set/reset the isUpdatedRemote flag for the field
                        level2.isUpdatedRemote = setreset;
                    }
                }
            }
        }
    }

    // Count the number of slashes in the key to determine the level of the key
    int slash_count = 0;
    for (char c : key)
    {
        if (c == '/')
        {
            slash_count++;
        }
    }

    // If there are no slashes, the key is at level 2
    if (slash_count == 0)
    {
        key_level2 = key;
    }
    // If there is one slash, the key is at level 3
    else if (slash_count == 1)
    {
        key_level2 = key.substr(0, key.find_first_of("/"));
    }
    // If there are more than one slashes in the key, it is not a valid key
    else
    {
        return;
    }

    // Search for the key in the dictionary structure and set/reset the isUpdatedLocal or isUpdatedRemote flag
    for (auto &level1 : dictionary_structure_)
    {
        if (level1.name == key_level1)
        {
            for (auto &level2 : level1.fields)
            {
                if (level2.name == key_level2)
                {
                    if (!localremote)
                    {
                        // Set/reset the isUpdatedLocal flag for the field
                        level2.isUpdatedLocal = setreset;
                    }
                    else
                    {
                        // Set/reset the isUpdatedRemote flag for the field
                        level2.isUpdatedRemote = setreset;
                    }
                    return;
                }
            }
        }
    }
}

void Dictionary::packetAdd(std::vector<uint8_t> &packet, const size_t offset, const size_t size)
{
    packet.reserve(packet.size() + 4 + size);
    packet.push_back(offset & 0xFF);
    packet.push_back(offset >> 8);
    packet.push_back(size & 0xFF);
    packet.push_back(size >> 8);
    packet.insert(packet.end(), dictionary_data_.begin() + offset, dictionary_data_.begin() + offset + size);
}

void Dictionary::packetProcessTransmit(std::vector<uint8_t> &packet)
{
    std::string key_level1 = "agent" + std::to_string(whoami_);

    // Clear the packet vector
    packet.clear();

    // Search for updated fields in the dictionary structure and add them to the transmit packet
    for (auto &level1 : dictionary_structure_)
    {
        if (level1.name == key_level1)
        {
            for (auto &level2 : level1.fields)
            {
                if (level2.isUpdatedLocal)
                {
                    // Add the updated field to the transmit packet
                    packetAdd(packet, level2.offset, level2.size);
                    // Reset the update flag for the updated field
                    level2.isUpdatedLocal = false;
                }
            }
        }
    }
}

void Dictionary::packetProcessReceive(const std::vector<uint8_t> packet)
{
    // Iterate over the data in the packet
    for (size_t index = 0; index < packet.size();)
    {
        // Get the offset and size of the data from the packet
        size_t offset = packet[index] | (packet[index + 1] << 8);
        size_t size = packet[index + 2] | (packet[index + 3] << 8);
        index += 4;

        // Copy the data from the packet to the dictionary data
        std::copy(packet.begin() + index, packet.begin() + index + size, dictionary_data_.begin() + offset);
        index += size;

        // Search for the updated field in the dictionary structure and set the update flag
        for (auto &level1 : dictionary_structure_)
        {
            for (auto &level2 : level1.fields)
            {
                if (level2.offset == offset)
                {
                    // Set the update flag for the updated field
                    level2.isUpdatedRemote = true;
                }
            }
        }
    }
}

void Dictionary::setDataToBeSent(std::string key, void *data_ptr)
{
    size_t offset, size;
    getOffsetSize(whoami_, key, offset, size);
    std::copy((uint8_t *)data_ptr, (uint8_t *)data_ptr + size, dictionary_data_.begin() + offset);
    setResetUpdate(whoami_, key, false, true);
}

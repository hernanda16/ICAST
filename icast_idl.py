# This is the idl to convert icast based dictionary into c header file

import json
import re
import sys

has_proceed_level2_key = []
header_name = ""
header_name_capslock = ""
path_to_json = ""

# ============== VOID INIT ====================

if len(sys.argv) <= 1:
    print("Usage python icast_idl.py <path_to_json>")
    exit(1)

path_to_json = sys.argv[1]

data = None
try:
    with open(path_to_json, 'r') as file:
        data = json.load(file)
except FileNotFoundError:
    print(f"Error: File not found - {path_to_json}")
    exit(1)
except json.JSONDecodeError as e:
    print(f"Error decoding JSON in {path_to_json}: {e}")
    exit(1)
except Exception as e:
    print(f"An unexpected error occurred: {e}")
    exit(1)

header_name = "icast_type"
header_name_capslock = header_name.upper()

# ===============================================

def extract_values(match):
    float_value = match.group(1)
    count_value = match.group(2) if match.group(2) else None
    return float_value, count_value

def custom_type_to_stdint_type(custom_type):
    ret_str = custom_type
    ret_arr_size = None

    pattern = r'(\w+)(?:\[(\d+)\])?'

    string_regex = re.match(pattern,custom_type)

    if string_regex:
        type_, ret_arr_size = extract_values(string_regex)
        if type_ == "uint1":
            ret_str = "uint8_t"
        elif type_ == "uint2":
            ret_str = "uint16_t"
        elif type_ == "uint4":
            ret_str = "uint32_t"
        elif type_ == "uint8":
            ret_str = "uint64_t"
        
        elif type_ == "int1":
            ret_str = "int8_t"
        elif type_ == "int2":
            ret_str = "int16_t"
        elif type_ == "int4":
            ret_str = "int32_t"
        elif type_ == "int8":
            ret_str = "int64_t"

        elif type_ == "float4":
            ret_str = "float"
        elif type_ == "float8":
            ret_str = "double"

        elif type_ == "string":
            ret_str = "string"
        
        else:
            print(f'{custom_type} is invalid icast type')
            exit(2)

    return ret_str, ret_arr_size


def generate_level2_struct(struct_name, fields):
    global has_proceed_level2_key

    # Filter level2 key
    if struct_name in has_proceed_level2_key:
        return ""

    struct_definition = f'typedef struct\n{{\n'
    for field, type_ in fields.items():
        extrc_type, arr_size = custom_type_to_stdint_type(type_)

        if arr_size == None:
            struct_definition += f'    {extrc_type} {field};\n'
        else:
            struct_definition += f'    {extrc_type} {field}[{arr_size}];\n'

    struct_definition += f'}} {struct_name}_t;\n\n'

    has_proceed_level2_key.append(struct_name)
    return struct_definition

def generate_level1_struct(struct_name, level2_keys):
    struct_definition = f'typedef struct\n{{\n'
    for level2_key in level2_keys:
        struct_definition += f'    {level2_key}_t {level2_key};\n'
    struct_definition += f'}} {struct_name}_t;\n\n'

    return struct_definition

def generate_c_header(json_data):
    header_content = f'#ifndef {header_name_capslock}_H\n#define {header_name_capslock}_H\n\n#include "stdint.h"\n\n'
    header_content += "#pragma pack(push, 1)\n\n"
    
    for k in json_data.keys():
        for l in json_data[k]: 
            header_content += generate_level2_struct(l,json_data[k][l])
    
    header_content += "//=============================================\n\n"
    
    for k in json_data.keys():
        level2_keys = []
        for l in json_data[k]: 
            level2_keys.append(l)
        header_content += generate_level1_struct(k,level2_keys)

    header_content += "//=============================================\n\n"
    
    header_content += generate_level1_struct('icast_bus', {k: f'{k}_t' for k in json_data.keys()})

    header_content += "#pragma pack(pop)"
    header_content += '\n#endif\n'
    
    return header_content

# ============ VOID MAIN ==================

# Generate C header
c_header = generate_c_header(data)

# Write it to a file 
with open("include/" +header_name + ".h", 'w') as f:
    f.write(c_header)


exit(0)
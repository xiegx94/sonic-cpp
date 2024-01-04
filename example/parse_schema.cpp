#include <iostream>
#include <string>

#include "sonic/sonic.h"

int main() {
  std::string json_schema = R"(
    {
      "a": 1,
      "b":{
        "c":1,
        "d":1.0
      }
    }
  )";
  
  std::string json = R"(
    {
      "a":3,
      "b": {
        "a":[[[]], []],
        "b":1,
        "c":3,
        "d":[[[]], [1], 1],
        "e":null,
        "f":false
      }
    }
  )";

  sonic_json::Document doc;
  doc.Parse(json_schema);

  doc.ParseSchema(json);
  std::cout << "schema a: " << doc["a"].GetInt64() << std::endl;
  std::cout << "schema : " << doc.Dump() << std::endl;

}
// g++ -I../include/ -march=haswell --std=c++11 parse_schema.cpp -o
// parse_schema

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
        "c":{"a":{"b":1}, "c":0, "d":[[[]]]},
        "d":[[[]], [1], 1],
        "e":null,
        "f":false
      },
      "c":0
    }
  )";

  sonic_json::Document doc;
  doc.Parse(json_schema);

  doc.ParseSchema(json);
  if (doc.HasParseError()) {
    std::cout << "json: " << json.substr(doc.GetErrorOffset()) << std::endl;
  }
  std::cout << "schema a: " << doc["a"].GetInt64() << std::endl;
  std::cout << "schema : " << doc.Dump() << std::endl;

}
// g++ -I../include/ -march=haswell --std=c++11 parse_schema.cpp -o
// parse_schema

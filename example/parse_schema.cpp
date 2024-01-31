#include <iostream>
#include <string>
#include <vector>

#include "sonic/sonic.h"

#if 0
static void json_path_create_doc(sonic_json::Document& doc, const std::vector<std::vector<std::string>>>& paths) {
    auto& alloc = doc.GetAllocator();
    for (const auto& path :  paths) {
        sonic_json::Node& node = doc;
        for (const auto& name : path) {
            if (!node.IsObject()) {
                node.SetObject();
            }
            auto m = node.FindMember(name);
            if (m == node.MemberEnd()) {
                node.AddMember(name, sonic_json::Node(sonic_json::kObject), alloc);
                m = node.FindMember(name);
                node = m->value;
            } else {
                node = m->value;
            }
        }
    }

    return;
}

#endif

int main() {
  std::string json_schema = R"(
  {"description_metas":1, "obj":1}
  )";
  
  std::string json = R"(
    []
  )";

  sonic_json::Document doc;
  doc.Parse(json_schema);
  if (doc.HasParseError()) std::cout << "error\n";
  // doc.Parse(json_schema);

  doc.ParseSchema(json);
  if (doc.HasParseError()) {
    std::cout << "json: " << json.substr(doc.GetErrorOffset()) << std::endl;
  }
  std::cout << "schema : " << doc.Dump() << std::endl;

}
// g++ -I../include/ -march=haswell --std=c++11 parse_schema.cpp -o
// parse_schema

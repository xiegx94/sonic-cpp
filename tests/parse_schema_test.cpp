/*
 * Copyright 2023 ByteDance Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "gtest/gtest.h"
#include "sonic/sonic.h"

namespace {

using namespace sonic_json;

void TestSuccess(const std::string schema, const std::string json, const std::string expect) {
    Document doc;
    doc.Parse(schema);
    EXPECT_FALSE(doc.HasParseError()) << "failed parsing schema: " << schema << std::endl;
    if (doc.HasParseError()) return;
    doc.ParseSchema(json);
    EXPECT_FALSE(doc.HasParseError()) << "failed parsing json: " << json << std::endl;
    if (doc.HasParseError()) return;
    Document expect_doc;
    expect_doc.Parse(expect);
    EXPECT_FALSE(doc.HasParseError()) << "failed parsing expect: " << expect << std::endl;
    if (expect_doc.HasParseError()) return;

    EXPECT_TRUE(doc == expect_doc);

}

TEST(ParseSchema, SuccessBasic) {
  TestSuccess(
    R"({"true": null, "false": null, "null":null, "int": null, "double":null, 
        "string": null, "object": null, "array": null
    })",
    R"({"true": true, "false": false, "null": null, "int": 1, "double": 1.0, "string": "string", 
        "object": {
            "object": {},
            "array": []
        },
        "array": [{}, []]
    })",
    R"({"true": true, "false": false, "null": null, "int": 1, "double": 1.0, "string": "string", 
        "object": {
            "object": {},
            "array": []
        },
        "array": [{}, []]
    })",
  )
}

}
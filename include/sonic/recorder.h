#pragma once

#include "sonic/sonic.h"

#define RETURN_NTH(_1,_2,_3,N,...) N

#define _R_FUNC(var) \
{\
  auto m = node.FindMember(#var);\
  if (m == node.MemberEnd()) {\
    X_ ## var = 0xFFFFFFFFFFFFFFFF;\
  } else {\
    X_ ## var = m - node.MemberBegin();\
  }\
}

#define _G_FUNC(var) \
Citer Get_ ## var(const sonic_json::Node& node) cosnt {\
  if (X_ ## var < node.Size()) {\
    auto m = node.MemberBegin() + X_ ## var;\
    if (#var == m->name.GetStringView()) { return m;}\
    else { return node.FindMember(#var); }\
  }\
  return node.MemberEnd();\
}

#define _U_FUNC(var) \
void Update_ ## var(size_t val) {X_ ## var = val; }\
void Update_ ## var(const sonic_json::Node& node) _R_RUNC(var)

#define _D_FUNC(var) size_t X_ ## var;

#define _R_0
#define _R_1(var) _R_FUNC(var)
#define _R_2(var,...) _R_FUNC(var) _R_1(__VA_ARGS__)
#define _R_3(var,...) _R_FUNC(var) _R_2(__VA_ARGS__)

#define _G_0
#define _G_1(var) _G_FUNC(var)
#define _G_2(var,...) _G_FUNC(var) _G_1(__VA_ARGS__)
#define _G_3(var,...) _G_FUNC(var) _G_2(__VA_ARGS__)

#define _U_0
#define _U_1(var) _U_FUNC(var)
#define _U_2(var,...) _U_FUNC(var) _U_1(__VA_ARGS__)
#define _U_3(var,...) _U_FUNC(var) _U_2(__VA_ARGS__)

#define _D_0
#define _D_1(var) _D_FUNC(var)
#define _D_2(var,...) _D_FUNC(var) _D_1(__VA_ARGS__)
#define _D_3(var,...) _D_FUNC(var) _D_2(__VA_ARGS__)

#define RECORDE_VARS(...) RETURN_NTH(__VA_ARGS__, _R_3, _R_2, _R_1, _R_0)(__VA_ARGS__)
#define GET_FUNCS(...) RETURN_NTH(__VA_ARGS__, _G_3, _G_2, _G_1, _G_0)(__VA_ARGS__)
#define UPDATE_FUNCS(...) RETURN_NTH(__VA_ARGS__, _U_3, _U_2, _U_1, _U_0)(__VA_ARGS__)
#define DECLEAR_VARS(...) RETURN_NTH(__VA_ARGS__, _D_3, _D_2, _D_1, _D_0)(__VA_ARGS__)

#define SONIC_DECLEAR_RECORDER(name, ...)\
struct name {\
  using Citer = typename sonic_json::Node::ConstMemberIterator;\
  bool Record(const sonic_json::Node& node) {\
    if (!node.IsObject()) { return false; }\
    RECORDE_VARS(__VA_ARGS__)\
    return true;\
  }\
  GET_FUNCS(__VA_ARGS__)\
  UPDATE_FUNCS(__VA_ARGS__)\
  DECLEAR_VARS(__VA_ARGS__)\
};

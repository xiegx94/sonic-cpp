import os
import argparse

License_test = """"""

Recorder_text = License_test + """
#pragma once

#include "sonic/sonic.h"

RETURN_NTH_PLACEHOLDER

#define _R_FUNC(var) \\
{\\
  auto m = node.FindMember(#var);\\
  if (m == node.MemberEnd()) {\\
    X_ ## var = 0xFFFFFFFFFFFFFFFF;\\
  } else {\\
    X_ ## var = m - node.MemberBegin();\\
  }\\
}

#define _G_FUNC(var) \\
Citer Get_ ## var(const sonic_json::Node& node) cosnt {\\
  if (X_ ## var < node.Size()) {\\
    auto m = node.MemberBegin() + X_ ## var;\\
    if (#var == m->name.GetStringView()) { return m;}\\
    else { return node.FindMember(#var); }\\
  }\\
  return node.MemberEnd();\\
}

#define _U_FUNC(var) \\
void Update_ ## var(size_t val) {X_ ## var = val; }\\
void Update_ ## var(const sonic_json::Node& node) _R_RUNC(var)

#define _D_FUNC(var) size_t X_ ## var {0xFFFFFFFFFFFFFFFF};

REOCRDE_PLACEHOLDER
GET_PLACEHOLDER
UPDATE_PLACEHOLDER
DECLEAR_PLACEHOLDER

#define SONIC_DECLEAR_RECORDER(name, ...)\\
struct name {\\
  using Citer = typename sonic_json::Node::ConstMemberIterator;\\
  name(const sonic_json::Node& node) { Record(node); }\\
  bool Record(const sonic_json::Node& node) {\\
    if (!node.IsObject()) { return false; }\\
    RECORDE_VARS(__VA_ARGS__)\\
    return true;\\
  }\\
  GET_FUNCS(__VA_ARGS__)\\
  UPDATE_FUNCS(__VA_ARGS__)\\
  DECLEAR_VARS(__VA_ARGS__)\\
};
"""

parser = argparse.ArgumentParser(
    description="Generate sonic reocrder files",
    formatter_class=argparse.RawDescriptionHelpFormatter,
    epilog="")
parser.add_argument('-n', '--max_size', type=int, default=128)
parser.add_argument('-o', '--output', type=str, default ='./recorder.h')


def return_n_placeholder(n):
    text = "#define RETURN_NTH("
    for i in range(1, n+1):
        text += "_" + str(i) + ", "

    text += "N, ...) N"
    return text

def func_placeholder(func_name, prefix_name, n):
    iter_text = "#define " + prefix_name + "_0\n" + \
                "#define " + prefix_name + "_1(var) " + prefix_name + "_FUNC(var)\n"
    func_text = "#define " + func_name + "(...) RETURN_NTH(__VA_ARGS__, "
    for i in range(2, n+1):
        item = "#define " + prefix_name + "_" + str(i) + "(var, ...) " + \
               prefix_name + "_FUNC(var) " + prefix_name + "_" + str(i-1) +"(__VA_ARGS__)\n"
        iter_text += item

        func_text += prefix_name + "_" + str(n + 2 - i) + ", "

    func_text += prefix_name + "_1, " + prefix_name + "_0)(__VA_ARGS__)\n"

    return iter_text + "\n" + func_text

def main(n, name):
    if n < 1:
        print("Max size must >= 1")
    global Recorder_text
    Recorder_text = Recorder_text.replace('RETURN_NTH_PLACEHOLDER', return_n_placeholder(n))
    Recorder_text = Recorder_text.replace('REOCRDE_PLACEHOLDER', func_placeholder('RECORDER_VARS', '_R', n))
    Recorder_text = Recorder_text.replace('GET_PLACEHOLDER', func_placeholder('GET_FUNCS', '_G', n))
    Recorder_text = Recorder_text.replace('UPDATE_PLACEHOLDER', func_placeholder('UPDATE_FUNCS', '_U', n))
    Recorder_text = Recorder_text.replace('DECLEAR_PLACEHOLDER', func_placeholder('DECLEAR_VARS', '_D', n))

    with open(name, 'w') as f:
        f.write(Recorder_text)

if __name__ == '__main__':
    args = parser.parse_args();
    main(args.max_size, args.output)


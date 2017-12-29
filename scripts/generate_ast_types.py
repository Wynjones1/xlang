#!/usr/bin/env python3
from pprint import pprint
import argparse
import jinja2

class Token(object):
    def __init__(self, value):
        self.value = value

data = [
    {"name" : "Top", "subtype" : "List", "fields" : []},
    {"name" : "Function", "fields" : [Token("name"), "args", "statements"]},
    {"name" : "ReturnStatement", "fields" : ["statement"]},
    {"name" : "FunctionArgs", "subtype" : "List"},
    {"name" : "StatementList", "subtype" : "List"},
    {"name" : "Statement"},
    {"name" : "Expression"},
]

def generate_class_source(type):
    sub_class  = type.get("subtype", "Ast")
    class_name = type["name"]
    out = ""
    out += f"""class {class_name} : public {sub_class}"""
    out += "{"
    out += "public:"
    # Generate the constructor.
    if len(type.get("fields", [])):
        out += f"{class_name}("
        for field in type.get("fields", []):
            if isinstance(field, Token):
                out += f"Token &{field.value},"
            else:
                out += f"AstPtr &{field},"
        out = out[:-1] + ") :"
        for field in type.get("fields", []):
            if isinstance(field, Token):
                out += f"{field.value}({field.value}),"
            else:
                out += f"{field}(std::move({field})),"
        out = out[:-1] + "{}"
    out += "\n"
    # Generate the print function.
    out += "std::string print(int indent = 0){"
    out += "auto out = std::string();"
    out += f"""out += "{class_name}\\n";"""
    for field in type.get("fields", []):
        if isinstance(field, Token):
            out += f"""out += std::string(indent, ' ') + std::string({field.value}.m_value) + "\\n";"""
        else:
            out += f"out += {field}->print(indent+1);"
    out += f"""out += "\\n"; """;
    out += "return out;"
    out += "}"
    # Declare the fields.
    for field in type.get("fields", []):
        if isinstance(field, Token):
            out += f"Token &{field.value};"
        else:
            out += f"AstPtr {field} = nullptr;"
    out += "};"
    return out

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("input")
    args = parser.parse_args()
    with open(args.input, "w") as fp:
        fp.write('#include "ast.h"\n\n')
        for type in data:
            fp.write(generate_class_source(type))
            fp.write("\n\n")

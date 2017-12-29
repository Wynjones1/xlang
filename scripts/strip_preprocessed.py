#!/usr/bin/env python3
import argparse
import re
import sys
import os
import subprocess
import tempfile

parser = argparse.ArgumentParser()
parser.add_argument("file")
args = parser.parse_args()

file_spec = re.compile(r"""# \d+ "([^"]+)".*""")
prefix = os.path.abspath(os.getcwd())
exclude = re.compile(".*dependencies.*")
keep = True
with tempfile.TemporaryDirectory() as dir:
    out_file = os.path.join(dir, "out.cpp")
    with open(out_file, "w") as out_fp:
        with open(args.file, "r") as fp:
            for line in fp:
                match = file_spec.match(line)
                if match:
                    file = match.group(1)
                    if prefix in file and not exclude.match(file):
                        keep = True
                    else:
                        keep = False
                elif keep:
                    out_fp.write(line)
    subprocess.check_call(["clang-format", out_file])

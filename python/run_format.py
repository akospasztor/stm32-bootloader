#!/usr/bin/env python
# -*- coding: utf-8 -*-
import argparse
import os
import subprocess
from common import collect_source_files


def run_format(source, style="file", executable="clang-format"):
    # Normalize executable path
    executable = os.path.normpath(executable)

    for s in source:
        clang_format_args = [executable]
        clang_format_args.append("-style={}".format(style))
        clang_format_args.append("-i")
        clang_format_args.append(os.path.basename(s))
        print("Formatting {}".format(s))
        output = subprocess.check_output(clang_format_args,
                                         cwd=os.path.dirname(s))
        if len(output) > 0:
            print(output)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="C/C++ formatting check using clang-format")

    # Style
    parser.add_argument("-s", "--style",
                        default="file",
                        help="Coding style, pass-through to clang-format's "
                        "-style=<string>, (default is '%(default)s').")

    # Specify executable for clang-format
    parser.add_argument("-e", "--executable",
                        default="clang-format",
                        help="Path of clang-format (if it's not added to PATH")
    args = parser.parse_args()

    file_list = collect_source_files()
    run_format(file_list, style=args.style, executable=args.executable)

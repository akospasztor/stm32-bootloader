#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import subprocess
from common import collect_source_files


def clang_format_run(source, style="file"):
    for s in source:
        clang_format_args = ["clang-format"]
        clang_format_args.append("-style={}".format(style))
        clang_format_args.append("-i")
        clang_format_args.append(os.path.basename(s))
        print("Formatting {}".format(s))
        output = subprocess.check_output(clang_format_args,
                                         cwd=os.path.dirname(s))
        if len(output) > 0:
            print(output)


if __name__ == "__main__":
    file_list = collect_source_files()
    clang_format_run(file_list)

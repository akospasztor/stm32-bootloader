#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os


def collect_source_files():
    # Source files extensions
    source_pattern = (".c", ".h", ".cpp", ".hpp")

    # Souce files, relative to project path
    source_list = ["lib/stm32-bootloader/",
                   "projects/"]

    # This module resides one folder down delative to project path, thus
    # the source list needs to be adjusted accordingly.
    source_list = [os.path.join(os.path.dirname(os.path.dirname(__file__)),
                                os.path.normpath(s))
                   for s in source_list]

    file_list = []
    for s in source_list:
        for root, dirs, files in os.walk(s):
            for f in files:
                if f.endswith(source_pattern):
                    file_list.append(os.path.join(root, f))
    return file_list

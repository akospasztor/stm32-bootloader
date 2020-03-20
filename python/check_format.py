#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import glob
import os
import subprocess
import traceback
import xml.etree.ElementTree as ET
from collections import namedtuple

__author__ = "github.com/cloderic"
__version__ = "0.3"

# Modifications and python3 compatibility added by:
# AKOS PASZTOR | github.com/akospasztor | 2020


def replacements_from_file(file, style="file", executable="clang-format"):
    Replacement = namedtuple("Replacement", "offset length text")
    replacements = []

    # Normalize executable path
    executable = os.path.normpath(executable)

    clang_format_args = [executable]
    clang_format_args.append("-style={}".format(style))
    clang_format_args.append("-output-replacements-xml")
    clang_format_args.append(os.path.basename(file))
    replacement_xml = subprocess.check_output(clang_format_args,
                                              cwd=os.path.dirname(file))
    replacement_xml_root = ET.XML(replacement_xml)
    for replacement_item in replacement_xml_root.findall('replacement'):
        replacements.append(Replacement(
            offset=int(replacement_item.attrib["offset"]),
            length=int(replacement_item.attrib["length"]),
            text=replacement_item.text
        ))

    return replacements


def errors_from_replacements(file, replacements=[]):
    Error = namedtuple("Error", "line column found expected")
    errors = []

    lines = [0]  # line index to character offset
    file_content = ""
    for line in open(file, "r"):
        file_content += line
        lines.append(lines[-1] + len(line))

    for line_index, line_offset in enumerate(lines[:-1]):
        while (len(replacements) > 0 and
               lines[line_index + 1] > replacements[0].offset):
            replacement = replacements.pop(0)
            errors.append(Error(
                line=line_index,
                column=replacement.offset - line_offset,
                found=file_content[replacement.offset:replacement.offset +
                                   replacement.length],
                expected=replacement.text if replacement.text else ""
            ))

        if len(replacements) == 0:
            break

    return errors


def check_format(files=[], style="file", executable="clang-format"):
    total_error_count = 0
    file_errors = dict()

    print("Collected {} files to check.".format(len(files)))
    for f in files:
        replacements = replacements_from_file(f, style, executable)
        errors = errors_from_replacements(f, replacements)
        if len(errors) > 0:
            print("- Checking {} ... {} format error(s)"
                  .format(f, len(errors)))
        else:
            print("- Checking {} ... ok".format(f))
        total_error_count += len(errors)
        file_errors[f] = errors
    if total_error_count == 0:
        print("No format errors found.")
    else:
        print("A total of {} format error(s) were found."
              .format(total_error_count))
    return total_error_count, file_errors


def check_clang_format_exec(executable="clang-format"):
    try:
        subprocess.check_output([executable, "--version"], shell=True)
        return True
    except subprocess.CalledProcessError:
        # it seems that in some version of clang-format '--version' leads to
        # non-zero exist status
        return True
    except OSError:
        return False


def main():
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
                        help="Path of clang-format if it's not added to PATH")

    # Files or directory to check
    parser.add_argument("file", nargs="+", help="Paths to the files that will "
                        "be checked (wilcards accepted).")
    args = parser.parse_args()

    try:
        # Adding the double quotes around the inline style
        if len(args.style) > 0 and args.style[0] == "{":
            args.style = "\"" + args.style + "\""

        # Checking that clang-format is available
        if not check_clang_format_exec(args.executable):
            print("Cannot run 'clang-format'. Please make sure the provided "
                  "executable is valid or `clang-format` can be reached in "
                  "PATH.")
            exit(-1)

        # Globing the file paths
        files = set()
        for pattern in args.file:
            for f in glob.iglob(pattern):
                files.add(os.path.relpath(f))
        file_list = list(files)
        error_count, file_errors = check_format(files=file_list,
                                                style=args.style,
                                                executable=args.executable)
        exit(error_count)

    except Exception as e:
        print("Exception raised:")
        print("    " + str(e))
        print('-' * 79)
        traceback.print_exc()
        print('-' * 79)
        exit(-2)


if __name__ == "__main__":
    main()

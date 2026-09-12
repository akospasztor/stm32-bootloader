#!/usr/bin/env python

"""Check and apply C/C++ formatting using clang-format.

This script can either verify that source files conform to a given coding style
or reformat them in-place. It requires the ``clang-format`` tool and parses its
XML replacement output to report per-file formatting errors.

:author:    Akos Pasztor
:copyright: (c) 2026 Akos Pasztor, https://akospasztor.com
:license:   This software is licensed under terms that can be found in the
            LICENSE file in the root directory of this software component.
"""

import argparse
import glob
import os
import subprocess
import xml.etree.ElementTree as ET


def replacements_from_file(file, style="file", executable="clang-format"):
    """Run clang-format on a file and return the list of replacements.

    :param file: Path to the source file.
    :type file:  str
    :param style: Coding style passed to clang-format's ``-style`` option.
    :type style:  str
    :param executable: Path to the clang-format executable.
    :type executable:  str
    :return: A list of dicts, each containing ``offset``, ``length``, and
             ``text`` keys describing a single replacement.
    :rtype:  list[dict]
    """
    replacements = []

    clang_format_args = [executable]
    clang_format_args.append("-style={}".format(style))
    clang_format_args.append("-output-replacements-xml")
    clang_format_args.append(os.path.basename(file))
    replacement_xml = subprocess.check_output(clang_format_args,
                                              cwd=os.path.dirname(file))
    replacement_xml_root = ET.XML(replacement_xml)
    for replacement_item in replacement_xml_root.findall('replacement'):
        replacements.append({
            "offset": int(replacement_item.attrib["offset"]),
            "length": int(replacement_item.attrib["length"]),
            "text": replacement_item.text,
        })

    return replacements


def errors_from_replacements(file, replacements):
    """Convert raw replacements into human-readable error descriptions.

    :param file: Path to the source file (used to read line offsets).
    :type file:  str
    :param replacements: List of replacement dicts as returned by
                         :func:`replacements_from_file`.
    :type replacements:  list[dict]
    :return: A list of dicts, each containing ``line``, ``column``, ``found``,
             and ``expected`` keys and values describing the formatting errors.
    :rtype:  list[dict]
    """
    errors = []

    lines = [0]  # line index to character offset
    file_content = ""
    for line in open(file, "r"):
        file_content += line
        lines.append(lines[-1] + len(line))

    for line_index, line_offset in enumerate(lines[:-1]):
        while (len(replacements) > 0 and
               lines[line_index + 1] > replacements[0]['offset']):
            replacement = replacements.pop(0)
            errors.append({
                "line": line_index + 1,
                "column": replacement['offset'] - line_offset + 1,
                "found": file_content[
                    replacement['offset']:replacement['offset'] +
                    replacement['length']],
                "expected": replacement['text'] if replacement['text'] else "",
            })

        if len(replacements) == 0:
            break

    return errors


def check_format(files, style="file", executable="clang-format"):
    """Check formatting of the given files and report errors.

    :param files: Iterable of file paths to check.
    :type files:  list[str]
    :param style: Coding style passed to clang-format's ``-style`` option.
    :type style:  str
    :param executable: Path to the clang-format executable.
    :type executable:  str
    :return: A tuple of ``(total_error_count, file_errors)`` where
             ``file_errors`` is a dict mapping file paths to their list of
             error dicts.
    :rtype:  tuple[int, dict]
    """
    executable = os.path.normpath(executable)
    total_error_count = 0
    file_errors = {}
    print("Collected {} file(s) to check.".format(len(files)))

    for f in files:
        print("- Checking {} ... ".format(f), end="")
        replacements = replacements_from_file(f, style, executable)
        errors = errors_from_replacements(f, replacements)
        if len(errors) > 0:
            print("{} format error{}".format(
                len(errors), 's' if len(errors) > 1 else ''))
            file_errors[f] = errors
            total_error_count += len(errors)
        else:
            print("OK")

    if total_error_count == 0:
        print("No format errors found.")
    else:
        print("A total of {} format error(s) found.".format(total_error_count))
        print("The following file(s) must be fixed:")
        for f in file_errors:
            print("- {}".format(f))
        print("Run clang-format to fix all format errors.")

    return total_error_count, file_errors


def run_format(files, style="file", executable="clang-format"):
    """Format the given files in-place using clang-format.

    :param files: Iterable of file paths to format.
    :type files:  list[str]
    :param style: Coding style passed to clang-format's ``-style`` option.
    :type style:  str
    :param executable: Path to the clang-format executable.
    :type executable:  str
    """
    executable = os.path.normpath(executable)
    print("Collected {} file(s) to format.".format(len(files)))

    for f in files:
        clang_format_with_args = [executable]
        clang_format_with_args.append("-style={}".format(style))
        clang_format_with_args.append("-i")
        clang_format_with_args.append(os.path.basename(f))
        print("- Formatting {}".format(f))
        output = subprocess.check_output(clang_format_with_args,
                                         cwd=os.path.dirname(f))
        if len(output) > 0:
            print(output)


def check_clang_format_exe(executable="clang-format"):
    """Verify that the clang-format executable is available.

    :param executable: Path to the clang-format executable.
    :type executable:  str
    :return: True if the executable can be invoked; otherwise False.
    :rtype:  bool
    """
    executable = os.path.normpath(executable)
    try:
        subprocess.check_output([executable, "--version"])
        return True
    except subprocess.CalledProcessError:
        # Some versions of clang-format --version lead to non-zero exit status
        return True
    except OSError:
        return False


def clang_format(files, check, style="file", executable="clang-format"):
    """Entry point: check or format the collected files.

    :param files: Iterable of file paths to process.
    :type files:  list[str]
    :param check: If True, only check formatting; otherwise format in-place.
    :type check:  bool
    :param style: Coding style passed to clang-format's ``-style`` option.
    :type style:  str
    :param executable: Path to the clang-format executable.
    :type executable:  str
    """
    # Add double quotes around inline style
    if len(style) > 0 and style[0] == "{":
        style = "\"" + style + "\""

    # Make sure that clang-format executable is available
    if not check_clang_format_exe(executable):
        print("Cannot run 'clang-format'. Please make sure the provided "
              "executable is valid or `clang-format` can be reached in PATH.")
        exit(-1)

    error_count = 0
    if check:
        error_count, _ = check_format(files=files,
                                      style=style,
                                      executable=executable)
    else:
        run_format(files=files,
                   style=style,
                   executable=executable)
    exit(error_count)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Check C/C++ formatting using clang-format")

    parser.add_argument('-c', '--check',
                        action='store_true',
                        help="Check the files instead of formatting them.")

    parser.add_argument("-s", "--style",
                        default="file",
                        help="Coding style, pass-through to clang-format's "
                        "-style=<string>, (default is '%(default)s').")

    parser.add_argument("-e", "--executable",
                        default="clang-format",
                        help="Path of clang-format if it's not added to PATH.")

    parser.add_argument("file", nargs="+",
                        help="Path(s) to the file(s) that will be checked "
                             "(wildcards and folders are accepted).")

    args = parser.parse_args()

    # Collect files
    collected_files = set()
    for path in args.file:
        # If a folder is specified, collect all files from that folder
        if os.path.isdir(path):
            path = os.path.join(path, '*.*')
        for f in glob.iglob(path):
            collected_files.add(os.path.abspath(f))
    # Sort collected files alphabetically
    collected_files = list(collected_files)
    collected_files.sort()

    clang_format(collected_files, args.check, args.style, args.executable)

#!/usr/bin/env python
# -*- coding: utf-8 -*-
from python.common import collect_source_files
from python.clang_format_check import clang_format_check


def test_clang_format():
    error_count, file_errors = clang_format_check(files=collect_source_files())
    assert error_count == 0

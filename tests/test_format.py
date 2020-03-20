#!/usr/bin/env python
# -*- coding: utf-8 -*-
from python.common import collect_source_files
from python.check_format import check_format


def test_clang_format(clang_format_executable):
    error_count, file_errors = check_format(files=collect_source_files(),
                                            executable=clang_format_executable)
    assert error_count == 0

#!/usr/bin/env python
# -*- coding: utf-8 -*-
from python.common import collect_source_files
from python.check_format import check_format


def test_clang_format():
    error_count, file_errors = check_format(files=collect_source_files())
    assert error_count == 0

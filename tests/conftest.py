#!/usr/bin/env python
# -*- coding: utf-8 -*-
import pytest


def pytest_addoption(parser):
    parser.addoption(
        "--executable", action="store", default="clang-format",
        help="Path of clang-format if it's not added to PATH"
    )


@pytest.fixture
def clang_format_executable(request):
    return request.config.getoption("--executable")

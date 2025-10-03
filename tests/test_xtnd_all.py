# Copyright 2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import pytest
from xtnd_test_util import discover_cases, run_xtnd_test

# Discover (stem, variant) tuples. variant may be None for base binaries.
CASES = discover_cases()

# Map of (stem, variant) to reason for expected failure.
XFAILS = {
}


def _case_id(param):
    stem, variant = param
    return stem if variant is None else f"{stem}[{variant}]"


IDS = [
    stem if variant is None else f"{stem}[{variant}]"
    for stem, variant in CASES
]

@pytest.mark.parametrize(
    "stem,variant",
    CASES,
    ids=IDS,
)
def test_xtnd_suite(stem, variant, capfd, verbosity, level):
    if (stem, variant) in XFAILS:
        pytest.xfail(XFAILS[(stem, variant)])
    assert run_xtnd_test(stem, variant, capfd, verbosity)

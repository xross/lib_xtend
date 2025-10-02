# Copyright 2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import pytest
from xtnd_test_util import discover_cases, run_xtnd_test

# Discover (stem, variant) tuples. variant may be None for base binaries.
CASES = discover_cases()

# Map of (stem, variant) to reason for expected failure.
XFAILS = {
    # Example: ("test_xtnd_xc_xc_timer", "t1"): "Known timing discrepancy on t1"
}


def _case_id(param):
    stem, variant = param
    return stem if variant is None else f"{stem}[{variant}]"


@pytest.mark.parametrize("case", CASES, ids=_case_id)
def test_xtnd_suite(case, capfd, verbosity, level):
    stem, variant = case
    if (stem, variant) in XFAILS:
        pytest.xfail(XFAILS[(stem, variant)])
    assert run_xtnd_test(stem, variant, capfd, verbosity)

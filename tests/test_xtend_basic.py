# Copyright 2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import pytest
import Pyxsim
from Pyxsim import testers
from pathlib import Path

def test_xtend_basic(level, capfd, verbosity):

    binary = Path(__file__).parent / "test_xtend_basic" / "bin" / "test_xtend_basic.xe"

    expect_file = Path(__file__).parent / "test_xtend_basic" / "pass.expect"

    tester = testers.ComparisonTester(open(expect_file), regexp=True, verbosity=verbosity)

    max_cycles = 15000000

    simargs = [
        "--max-cycles",
        str(max_cycles),
    ]

    result = Pyxsim.run_on_simulator(
        binary,
        cmake=True,
        simargs=simargs,
        tester=tester,
        capfd=capfd,
        clean_before_build=False)

    assert result

# Copyright 2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

#note, this test fails since program exit is not detected

import pytest
import Pyxsim
from Pyxsim import testers
from pathlib import Path

@pytest.mark.xfail(reason="If system calls used in plugin cannot be used in host app")
def test_xtend_global(level, capfd, verbosity):

    binary = Path(__file__).parent / "test_xtend_print" / "bin" / "test_xtend_print.xe"

    expect_file = Path(__file__).parent / "test_xtend_print" / "pass.expect"

    tester = testers.ComparisonTester(open(expect_file), regexp=True, verbosity=verbosity)

    max_cycles = 15000000

    simargs = [
        "--max-cycles",
        str(max_cycles),
         "--syscall-address", "tile[0]", "0x821c0", # NOTE THIS IS EXREMELY FRAGILE!
    ]

    result = Pyxsim.run_on_simulator(
        binary,
        cmake=True,
        simargs=simargs,
        tester=tester,
        capfd=capfd,
        clean_before_build=False)

    assert result

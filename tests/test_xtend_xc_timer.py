# Copyright 2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

import pytest
import Pyxsim
from Pyxsim import testers
from pathlib import Path

# t1 variant currently expected to fail (marked xfail until fixed)
VARIANTS = [
    pytest.param("t0", id="t0"),
    pytest.param("t1", id="t1", marks=pytest.mark.xfail(reason="Known issue: t1 variant currently failing")),
]

@pytest.mark.parametrize("variant", VARIANTS)
def test_xtend_xc_timer(level, capfd, verbosity, variant):
    variant_binary = f"test_xtend_xc_timer_{variant}.xe"
    binary = Path(__file__).parent / "test_xtend_xc_timer" / "bin" / variant /variant_binary

    expect_file = Path(__file__).parent / "test_xtend_xc_timer" / "pass.expect"

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
        clean_before_build=False,
    )

    assert result

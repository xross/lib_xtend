# Copyright 2025 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.

from pathlib import Path
from Pyxsim import testers, run_on_simulator

DEFAULT_MAX_CYCLES = 15_000_000
VARIANTS = ("t0", "t1")  # Supported variant names


def discover_cases(prefix: str = "test_xtnd_"):
    """Discover test (stem, variant) execution cases.

    For each directory under this file's parent whose name starts with the
    given prefix, inspect its bin/ layout to determine whether variant
    binaries exist. Variant search order for a stem S and variant V is:
      1. bin/V/S_V.xe
      2. bin/V/S.xe
      3. bin/S_V.xe

    If any variant binary is found for a stem, ONLY those variant cases are
    returned (base binary ignored even if present). Otherwise if no variant
    binaries exist, a single (stem, None) case is returned provided the base
    binary bin/S.xe exists.

    Returns a sorted list of (stem, variant) tuples where variant may be None.
    """
    base = Path(__file__).parent
    cases = []
    for d in base.iterdir():
        if not d.is_dir():
            continue
        stem = d.name
        if not stem.startswith(prefix):
            continue

        # Attempt to locate variant binaries
        variant_found = False
        for v in VARIANTS:
            vb = _resolve_variant_binary_path(d, stem, v)
            if vb is not None:
                cases.append((stem, v))
                variant_found = True
        if variant_found:
            continue  # Do not add base case when variants present

        # Fallback to base binary
        base_bin = d / "bin" / f"{stem}.xe"
        if base_bin.exists():
            cases.append((stem, None))
    # Sort with base (None) variants first for a stem
    cases.sort(key=lambda t: (t[0], "" if t[1] is None else t[1]))
    return cases


def _resolve_variant_binary_path(stem_dir: Path, stem: str, variant: str):
    """Return Path to first existing binary for (stem, variant) or None.

    Search order matches discover_cases documentation. Does not assert.
    """
    candidates = [
        stem_dir / "bin" / variant / f"{stem}_{variant}.xe",
        stem_dir / "bin" / variant / f"{stem}.xe",
        stem_dir / "bin" / f"{stem}_{variant}.xe",
    ]
    for c in candidates:
        if c.exists():
            return c
    return None


def discover_stems(prefix: str = "test_xtnd_"):
    """Deprecated: retained for backward compatibility.

    Returns stems that have a base (non-variant) binary only. Prefer
    discover_cases() for new code.
    """
    return sorted({stem for stem, variant in discover_cases(prefix) if variant is None})


def run_xtnd_test(stem: str, variant, capfd, verbosity: int, *, max_cycles: int = DEFAULT_MAX_CYCLES, extra_simargs=None):
    """Run a single xtnd test binary identified by stem and optional variant.

    Layouts considered (in resolution order when variant provided V):
      tests/<stem>/bin/V/<stem>_V.xe
      tests/<stem>/bin/V/<stem>.xe
      tests/<stem>/bin/<stem>_V.xe
    Base (non-variant) layout:
      tests/<stem>/bin/<stem>.xe

    A single shared expect file tests/pass.expect is used.
    """
    stem_dir = Path(__file__).parent / stem
    if variant is None:
        binary = stem_dir / "bin" / f"{stem}.xe"
    else:
        binary = _resolve_variant_binary_path(stem_dir, stem, variant)
        assert binary is not None, f"Missing variant binary for {stem} variant {variant}"

    expect_file = Path(__file__).parent / "pass.expect"

    assert binary.exists(), f"Missing binary: {binary}"
    assert expect_file.exists(), f"Missing global expect file: {expect_file}"

    with open(expect_file, "r", encoding="utf-8") as fh:
        tester = testers.ComparisonTester(fh, regexp=True, verbosity=verbosity)

        simargs = ["--max-cycles", str(max_cycles)]
        if extra_simargs:
            simargs += list(extra_simargs)

        result = run_on_simulator(
            binary,
            cmake=True,
            simargs=simargs,
            tester=tester,
            capfd=capfd,
            clean_before_build=False,
        )

    return result

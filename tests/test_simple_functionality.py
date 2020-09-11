#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import unittest
import os
from subprocess import check_call, CalledProcessError
from unittest.runner import TextTestRunner

ROOT = os.path.join(os.path.dirname(__file__), "..")
BIN = os.path.join(ROOT, "build/meson.debug.linux.x86_64/src/packetClassificators")


class SimpleFunctionalityTC(unittest.TestCase):
    DEFAULT_RULESET = os.path.join(ROOT, "tests/rulesets/acl1_100")
    
    def run_bin(self, alg, ruleset=None):
        if ruleset is None:
            ruleset = self.DEFAULT_RULESET

        check_call([BIN, f"c={alg}", f"f={ruleset}", "r=2"])

    def test_fail(self):
        with self.assertRaises((AssertionError, CalledProcessError)):
            self.run_bin("unknownAlg")
    
    def test_PartitionSort(self):
        self.run_bin("PartitionSort")

    def test_PriorityTupleSpaceSearch(self):
        self.run_bin("PriorityTupleSpaceSearch")

    def test_HyperSplit(self):
        self.run_bin("HyperSplit")

    def test_HyperCuts(self):
        self.run_bin("HyperCuts")

    def test_ByteCuts(self):
        self.run_bin("ByteCuts")

    def test_BitVector(self):
        self.run_bin("BitVector")

    def test_TupleSpaceSearch(self):
        self.run_bin("TupleSpaceSearch")
        
    def test_TupleMergeOnline(self):
        self.run_bin("TupleMergeOnline")
    
    def test_TupleMergeOffline(self):
        self.run_bin("TupleMergeOffline")

    def test_pcv(self):
        self.run_bin("pcv")


class ValidationTC(SimpleFunctionalityTC):
    
    def run_bin(self, alg, ruleset=None):
        if ruleset is None:
            ruleset = self.DEFAULT_RULESET
        cmd = [BIN, f"c={alg}", f"f={ruleset}", "m=Validation"]
        try:
            check_call(cmd)
        except CalledProcessError:
            raise AssertionError(" ".join(cmd), "failed")

if __name__ == "__main__":
    suite = unittest.TestSuite()
    # suite.addTest(SimpleFunctionalityTC('test_sWithStartPadding'))
    for tc in [SimpleFunctionalityTC, ValidationTC]:
        suite.addTest(unittest.makeSuite(tc))

    # runner = TextTestRunner(verbosity=2, failfast=True)
    runner = TextTestRunner(verbosity=2)

    try:
        from concurrencytest import ConcurrentTestSuite, fork_for_tests
        useParallerlTest = True
    except ImportError:
        # concurrencytest is not installed, use regular test runner
        useParallerlTest = False
    # useParallerlTest = False
   
    if useParallerlTest:
        concurrent_suite = ConcurrentTestSuite(suite, fork_for_tests())
        runner.run(concurrent_suite)
    else:
        runner.run(suite)

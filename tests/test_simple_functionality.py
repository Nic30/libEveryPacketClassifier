#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import unittest
import os
from subprocess import check_call, CalledProcessError

ROOT = os.path.join(os.path.dirname(__file__), "..")
BIN = os.path.join(ROOT, "Debug/packetClassificators")


class SimpleFunctionalityTC(unittest.TestCase):
    DEFAULT_RULESET = os.path.join(ROOT, "tests/rulesets/acl1_100")
    
    @classmethod
    def setUpClass(cls):
        check_call(["make", "-C" , os.path.dirname(BIN)])
    
    def run_bin(self, alg, ruleset=None):
        if ruleset is None:
            ruleset = self.DEFAULT_RULESET

        check_call([BIN, f"c={alg}", f"f={ruleset}"])

    def test_fail(self):
        with self.assertRaises(CalledProcessError):
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
    # suite.addTest(unittest.makeSuite(SimpleFunctionalityTC))
    suite.addTest(unittest.makeSuite(ValidationTC))
    
    runner = unittest.TextTestRunner(verbosity=3)
    runner.run(suite)

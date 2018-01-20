# Distributed under the MIT software license, see the accompanying
# file LICENSE or http://www.opensource.org/licenses/mit-license.php.
from unittest import TestCase
import time

from pyqryptonight.pyqryptonight import Qryptominer
from pyqryptonight.pyqryptonight import Qryptonight
from pyqryptonight.pyqryptonight import PoWHelper


class TestQryptominer(TestCase):
    def __init__(self, *args, **kwargs):
        super(TestQryptominer, self).__init__(*args, **kwargs)

    def test_miner_simple(self):
        class CustomQMiner(Qryptominer):
            def __init__(self):
                Qryptominer.__init__(self)

            def solutionEvent(self, nonce):
                print("Hey a solution has been found!", nonce)
                self.python_nonce = nonce

        input_bytes = [0x03, 0x05, 0x07, 0x09, 0x19]
        target = [
            0x1E, 0xE5, 0x3F, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
            0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
            0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
            0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0xFF
        ]

        # Create a customized miner
        qm = CustomQMiner()

        # Set input bytes, nonce
        qm.start(input=input_bytes,
                 nonceOffset=0,
                 target=target,
                 thread_count=2)

        # Python can sleep or do something else.. the callback will happen in the background
        time.sleep(2)

        # This property has been just created in the python custom class when the event is received
        self.assertEqual(2, qm.python_nonce)

        # Now check wrapper values
        self.assertEqual(True, qm.solutionAvailable())
        self.assertEqual(2, qm.solutionNonce())

    def test_miner_verify(self):
        class CustomQMiner(Qryptominer):
            def __init__(self):
                Qryptominer.__init__(self)

            def solutionEvent(self, nonce):
                print("Hey a solution has been found!", nonce)
                self.python_nonce = nonce

        input_bytes = [0x03, 0x05, 0x07, 0x09, 0x19]
        target = [
            0x1E, 0xE5, 0x3F, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
            0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
            0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
            0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0xFF
        ]

        # Create a customized miner
        qm = CustomQMiner()

        # Set input bytes, nonce
        qm.start(input=input_bytes,
                 nonceOffset=0,
                 target=target,
                 thread_count=2)

        # Python can sleep or do something else.. the callback will happen in the background
        time.sleep(2)

        # This property has been just created in the python custom class when the event is received
        self.assertEqual(2, qm.python_nonce)
        self.assertEqual(True, qm.solutionAvailable())
        self.assertEqual(2, qm.solutionNonce())

        solution_input = list(qm.solutionInput())

        print("input_bytes    ", input_bytes)
        print("solution_input ", solution_input)
        print("target         ", target)
        print("solutionHash   ", qm.solutionHash())

        qn = Qryptonight()
        output = qn.hash(solution_input)
        print("raw     Hash   ", output)

        self.assertTrue(PoWHelper.verifyInput(solution_input, target))
        solution_input[4] = 0x20
        self.assertFalse(PoWHelper.verifyInput(solution_input, target))

# Distributed under the MIT software license, see the accompanying
# file LICENSE or http://www.opensource.org/licenses/mit-license.php.
from unittest import TestCase
import time

from pyqryptonight import pyqryptonight
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

            def handleEvent(self, event):
                if event.type == pyqryptonight.SOLUTION:
                    print("Hey a solution has been found!", event.nonce)
                    self.python_nonce = event.nonce
                return True

        input_bytes = [
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09
        ]
        target = [
            0x1E, 0xE5, 0x3F, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
            0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
            0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
            0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0x00
        ]

        # Create a customized miner
        qm = CustomQMiner()

        # Set input bytes, nonce
        qm.start(input=input_bytes,
                 nonceOffset=0,
                 target=target,
                 thread_count=2)

        # Python can sleep or do something else.. the callback will happen in the background
        # Use waitForAnswer instead of fixed sleep to ensure we wait until solution is found
        result = qm.waitForAnswer(30)  # Wait up to 30 seconds for solution
        
        # This property has been just created in the python custom class when the event is received
        self.assertTrue(result, "No solution found within timeout")
        self.assertEqual(206, qm.python_nonce)

        # Now check wrapper values
        self.assertEqual(True, qm.solutionAvailable())
        self.assertEqual(206, qm.solutionNonce())

    def test_miner_verify(self):
        class CustomQMiner(Qryptominer):
            def __init__(self):
                Qryptominer.__init__(self)

            def handleEvent(self, event):
                if event.type == pyqryptonight.SOLUTION:
                    print("Hey a solution has been found!", event.nonce)
                    self.python_nonce = event.nonce
                return True

        input_bytes = [
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09
        ]
        target = [
            0x1E, 0xE5, 0x3F, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
            0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
            0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
            0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0x0C
        ]

        # Create a customized miner
        qm = CustomQMiner()

        # Set input bytes, nonce
        qm.start(input=input_bytes,
                 nonceOffset=0,
                 target=target,
                 thread_count=2)

        # Python can sleep or do something else.. the callback will happen in the background
        time.sleep(5)

        # This property has been just created in the python custom class when the event is received
        self.assertEqual(37, qm.python_nonce)
        self.assertEqual(True, qm.solutionAvailable())
        self.assertEqual(37, qm.solutionNonce())

        solution_input = list(qm.solutionInput())

        print("input_bytes    ", input_bytes)
        print("solution_input ", solution_input)
        print("target         ", target)
        print("solutionHash   ", qm.solutionHash())

        qn = Qryptonight()
        output = qn.hash(solution_input)
        print("raw     Hash   ", output)

        ph = PoWHelper()
        self.assertTrue(ph.verifyInput(solution_input, target))
        solution_input[4] = 0x29
        self.assertFalse(ph.verifyInput(solution_input, target))

    def test_miner_timeout(self):
        class CustomQMiner(Qryptominer):
            def __init__(self):
                Qryptominer.__init__(self)
                self.timeout_triggered = False

            def handleEvent(self, event):
                if event.type == pyqryptonight.TIMEOUT:
                    print("Timeout")
                    self.timeout_triggered = True
                return True

        input_bytes = [
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09
        ]
        target = [
            0x1E, 0xE5, 0x3F, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
            0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
            0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
            0xF8, 0xF9, 0xE8, 0x9D, 0x00, 0x00, 0x00, 0x00
        ]

        # Create a customized miner
        qm = CustomQMiner()

        # Set input bytes, nonce
        qm.start(input=input_bytes,
                 nonceOffset=0,
                 target=target,
                 thread_count=2)

        qm.setTimer(2000)

        # Python can sleep or do something else.. the callback will happen in the background
        time.sleep(5)

        # This property has been just created in the python custom class when the event is received
        self.assertTrue(qm.timeout_triggered)

    def test_miner_timeout_extend(self):
        class CustomQMiner(Qryptominer):
            def __init__(self):
                Qryptominer.__init__(self)
                self.timeout_triggered = False

            def handleEvent(self, event):
                if event.type == pyqryptonight.TIMEOUT:
                    print("Timeout")
                    self.timeout_triggered = True
                return True

        input_bytes = [
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09
        ]
        target = [
            0x1E, 0xE5, 0x3F, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
            0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
            0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
            0xF8, 0xF9, 0xE8, 0x9D, 0x00, 0x00, 0x00, 0x00
        ]

        # Create a customized miner
        qm = CustomQMiner()

        # Set input bytes, nonce
        qm.start(input=input_bytes,
                 nonceOffset=0,
                 target=target,
                 thread_count=2)

        qm.setTimer(2000)
        time.sleep(1)
        qm.setTimer(4000)
        time.sleep(3)

        # This property has been just created in the python custom class when the event is received
        self.assertFalse(qm.timeout_triggered)

    def test_miner_timeout_disable(self):
        class CustomQMiner(Qryptominer):
            def __init__(self):
                Qryptominer.__init__(self)
                self.timeout_triggered = False

            def handleEvent(self, event):
                if event.type == pyqryptonight.TIMEOUT:
                    print("Timeout")
                    self.timeout_triggered = True
                return True

        input_bytes = [
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09
        ]
        target = [
            0x1E, 0xE5, 0x3F, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
            0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
            0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
            0xF8, 0xF9, 0xE8, 0x9D, 0x00, 0x00, 0x00, 0x00
        ]

        # Create a customized miner
        qm = CustomQMiner()

        # Set input bytes, nonce
        qm.start(input=input_bytes,
                 nonceOffset=0,
                 target=target,
                 thread_count=2)

        qm.setTimer(500)
        qm.disableTimer()
        time.sleep(1)

        # This property has been just created in the python custom class when the event is received
        self.assertFalse(qm.timeout_triggered)

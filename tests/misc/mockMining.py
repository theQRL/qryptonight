# Distributed under the MIT software license, see the accompanying
# file LICENSE or http://www.opensource.org/licenses/mit-license.php.

from pyqryptonight.pyqryptonight import StringToUInt256, UInt256ToString
from pyqryptonight.pyqryptonight import PoWHelper
import time
from pyqryptonight.pyqryptonight import Qryptominer


class CustomQMiner(Qryptominer):
    def __init__(self):
        Qryptominer.__init__(self)

    def solutionEvent(self, nonce):
        self.end = time.time()


def main():
    ph = PoWHelper()
    qm = CustomQMiner()

    input_bytes = [0x03, 0x05, 0x07, 0x09, 0x19]
    difficulty = StringToUInt256("5000")

    for i in range(10):
        boundary = ph.getBoundary(difficulty)

#       print("difficulty     ", difficulty)
        print("difficulty str ", UInt256ToString(difficulty))
        print("boundary       ", boundary)
#        print("boundary str   ", UInt256ToString(boundary))

        start = time.time()

        # Set input bytes, nonce
        qm.setInput(input=input_bytes,
                    nonceOffset=0,
                    target=boundary)

        qm.start(thread_count=2)

        while not qm.solutionFound():
            time.sleep(1)

        print("time           ", qm.end - start)
        print("hash           ", qm.solutionHash())
        print()

        # Set a new difficulty
        difficulty = ph.getDifficulty(int(qm.end),
                                      int(start),
                                      difficulty)


if __name__ == '__main__':
    main()

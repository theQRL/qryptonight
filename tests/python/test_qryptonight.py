# Distributed under the MIT software license, see the accompanying
# file LICENSE or http://www.opensource.org/licenses/mit-license.php.
from unittest import TestCase

from pyqryptonight.pyqryptonight import Qryptonight


class TestQryptonight(TestCase):
    def __init__(self, *args, **kwargs):
        super(TestQryptonight, self).__init__(*args, **kwargs)

    def test_init(self):
        qn = Qryptonight()

    def test_hash(self):
        qn = Qryptonight()

        input = [0x03, 0x05, 0x07, 0x09]
        output = qn.hash(input)

        print(output)

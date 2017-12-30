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

        output_expected = (
            0x3E, 0xE5, 0x3F, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
            0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
            0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
            0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0xF6
        )

        self.assertEqual(output_expected, output)

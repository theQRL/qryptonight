# Distributed under the MIT software license, see the accompanying
# file LICENSE or http://www.opensource.org/licenses/mit-license.php.
from unittest import TestCase

from pyqryptonight.pyqryptonight import StringToUInt256, UInt256ToString


class TestStr2BigNum(TestCase):
    def __init__(self, *args, **kwargs):
        super(TestStr2BigNum, self).__init__(*args, **kwargs)

    def test_number16(self):
        number = StringToUInt256("16")

        expected_vector = (
            0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        )

        self.assertEqual(expected_vector, number)

    def test_numberBig(self):
        number = StringToUInt256("134123412341234123412346")

        expected_vector = (
             122,   19,  248,  230,   14,  172,   65,  216,
             102,   28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        )

        self.assertEqual(expected_vector, number)

    def test_numberBack(self):
        input_vector = (
            122,   19,  248,  230,   14,  172,   65,  216,
            102,   28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        )

        value = UInt256ToString(input_vector)

        expected_value = "134123412341234123412346"

        self.assertEqual(expected_value, value)

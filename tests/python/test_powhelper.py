# Distributed under the MIT software license, see the accompanying
# file LICENSE or http://www.opensource.org/licenses/mit-license.php.
from unittest import TestCase
from pyqryptonight.pyqryptonight import StringToUInt256, UInt256ToString

from pyqryptonight.pyqryptonight import PoWHelper


class TestPowHelper(TestCase):
    def __init__(self, *args, **kwargs):
        super(TestPowHelper, self).__init__(*args, **kwargs)

    def test_boundary(self):
        ph = PoWHelper()

        difficulty = (
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x42, 0x40
        )

        boundary = ph.getBoundary(difficulty)

        expected_boundary = (
            0, 0, 16, 198, 247, 160, 181, 237,
            141, 54, 180, 199, 243, 73, 56, 88,
            54, 33, 250, 252, 139, 0, 121, 162,
            131, 77, 38, 250, 63, 204, 158, 169
        )

        self.assertEqual(expected_boundary, boundary)

    def test_adaptive_boundary(self):
        ph = PoWHelper()

        parent_difficulty = StringToUInt256("5000")

        current_difficulty = ph.getDifficulty(measurement=104,
                                              parent_difficulty=parent_difficulty)

        expected_difficulty = '4644'

        print(parent_difficulty)
        print(expected_difficulty)
        print(current_difficulty)

        self.assertEqual(expected_difficulty, UInt256ToString(current_difficulty))

        boundary = ph.getBoundary(current_difficulty)
        expected_boundary = "24933697079525451210932597977753640795277774475805461679469763998258641180"

        self.assertEqual(expected_boundary, UInt256ToString(boundary))

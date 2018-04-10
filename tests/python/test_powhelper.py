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

        target = ph.getTarget(difficulty)

        expected_target = (
            169, 158, 204, 63, 250, 38, 77, 131,
            162, 121, 0, 139, 252, 250, 33, 54,
            88, 56, 73, 243, 199, 180, 54, 141,
            237, 181, 160, 247, 198, 16, 0, 0
        )

        self.assertEqual(expected_target, target)

    def test_adaptive_target(self):
        ph = PoWHelper()

        parent_difficulty = StringToUInt256("5000")

        current_difficulty = ph.getDifficulty(measurement=104,
                                              parent_difficulty=parent_difficulty)

        expected_difficulty = '4644'

        print(parent_difficulty)
        print(expected_difficulty)
        print(current_difficulty)

        self.assertEqual(expected_difficulty, UInt256ToString(current_difficulty))

        target = ph.getTarget(current_difficulty)
        expected_target = "12766941454368345787240450318120704813017110301439674670851728194227068997120"

        self.assertEqual(expected_target, UInt256ToString(target))

    def test_target_1(self):
        ph = PoWHelper()

        difficulty = (
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x42, 0x40
        )

        target = ph.getTarget(difficulty)

        expected_target = (
            169, 158, 204, 63, 250, 38, 77, 131,
            162, 121, 0, 139, 252, 250, 33, 54,
            88, 56, 73, 243, 199, 180, 54, 141,
            237, 181, 160, 247, 198, 16, 0, 0
        )

        self.assertEqual(expected_target, target)

    def test_target_2(self):
        ph = PoWHelper(kp=0, set_point=0)
        val = ph.getKp()
        print(val)

        # difficulty = (
        #     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        #     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        #     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        #     0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x42, 0x40
        # )
        #
        # target = ph.getTarget(difficulty)
        #
        # expected_target = (
        #     169, 158, 204, 63, 250, 38, 77, 131,
        #     162, 121, 0, 139, 252, 250, 33, 54,
        #     88, 56, 73, 243, 199, 180, 54, 141,
        #     237, 181, 160, 247, 198, 16, 0, 0
        # )
        #
        # self.assertEqual(expected_target, target)

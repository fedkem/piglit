#
# Minimal tests to check whether the installation is working
#

from framework.profile import TestProfile
from framework.exectest import PiglitTest

__all__ = ['profile']

profile = TestProfile()
profile.tests['spec/arb_shader_precision-divide-by-zero'] = PiglitTest('arb_shader_precision-divide-by-zero')

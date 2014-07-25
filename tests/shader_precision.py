#
# Minimal tests to check whether the installation is working
#

from framework.profile import TestProfile
from framework.exectest import PiglitTest

__all__ = ['profile']

profile = TestProfile()
profile.tests['spec/arb_shader_precision-divide-by-zero'] = PiglitTest('arb_shader_precision-divide-by-zero')
profile.tests['spec/arb_shader_precision-denormalized-input'] = PiglitTest('arb_shader_precision-denormalized-input')
profile.tests['spec/arb_shader_precision-denormalized-output'] = PiglitTest('arb_shader_precision-denormalized-output')
profile.tests['spec/arb_shader_precision-add'] = PiglitTest('arb_shader_precision-add')

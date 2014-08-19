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
profile.tests['spec/arb_shader_precision-sub'] = PiglitTest('arb_shader_precision-sub')
profile.tests['spec/arb_shader_precision-mult'] = PiglitTest('arb_shader_precision-mult')
profile.tests['spec/arb_shader_precision-fma-manual'] = PiglitTest('arb_shader_precision-fma-manual')
profile.tests['spec/arb_shader_precision-log'] = PiglitTest('arb_shader_precision-log')
profile.tests['spec/arb_shader_precision-lt'] = PiglitTest('arb_shader_precision-lt')
profile.tests['spec/arb_shader_precision-lte'] = PiglitTest('arb_shader_precision-lte')
profile.tests['spec/arb_shader_precision-equal'] = PiglitTest('arb_shader_precision-equal')
profile.tests['spec/arb_shader_precision-gt'] = PiglitTest('arb_shader_precision-gt')
profile.tests['spec/arb_shader_precision-gte'] = PiglitTest('arb_shader_precision-gte')
profile.tests['spec/arb_shader_precision-pow'] = PiglitTest('arb_shader_precision-pow')

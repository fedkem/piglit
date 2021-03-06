#
# Testing the r300 DRI driver
#

from tests.quick import profile

__all__ = ['profile']

# glean/blendFunc
#   R300 blending hardware appears to be bad
env = profile.tests['glean']['blendFunc'].env
env['GLEAN_BLEND_RGB_TOLERANCE'] = '1.9'
env['GLEAN_BLEND_ALPHA_TOLERANCE'] = '2.0'

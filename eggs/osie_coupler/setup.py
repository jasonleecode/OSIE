"""
Install OSIE's coupler.
"""
from setuptools import setup

setup(
    name='osie_coupler',
    version='0.1',
    entry_points={'console_scripts': ['osie_plc_server = osie_coupler.osie_coupler:main']},
    install_requires=['pyA20Lime2', 'pymodbus[twisted]', 'click', 'prompt_toolkit', 'pygments', 'bitarray']
)

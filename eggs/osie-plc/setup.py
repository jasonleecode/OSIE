"""
Install OSIE-PLC.
"""
from setuptools import setup

setup(
    name='osie-plc',
    version='0.1',
    entry_points={'console_scripts': ['modbus_server = modbus_server:main']},
    install_requires=['pyA20Lime2', 'pymodbus[twisted]', 'click', 'prompt_toolkit', 'pygments']
)

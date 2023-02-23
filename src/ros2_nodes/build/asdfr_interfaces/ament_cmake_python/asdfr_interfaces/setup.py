from setuptools import find_packages
from setuptools import setup

setup(
    name='asdfr_interfaces',
    version='0.0.0',
    packages=find_packages(
        include=('asdfr_interfaces', 'asdfr_interfaces.*')),
)

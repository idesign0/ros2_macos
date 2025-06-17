from setuptools import find_packages
from setuptools import setup

package_name = 'rpyutils'

setup(
    name=package_name,
    version='0.2.1',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/' + package_name, ['package.xml']),
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
    ],
    maintainer='Jacob Perron',
    maintainer_email='jacob@openrobotics.org',
    url='https://github.com/ros2/rpyutils',
    download_url='https://github.com/ros2/rpyutils/releases',
    keywords=[],
    classifiers=[
        'Environment :: Console',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: Apache Software License',
        'Programming Language :: Python',
    ],
    description='Python utilities for the ROS 2 code base.',
    long_description=(
        'This package provides types and functions for common operations '
        'throughout the ROS 2 code base.'),
    license='Apache License, Version 2.0',
    tests_require=['pytest'],
)

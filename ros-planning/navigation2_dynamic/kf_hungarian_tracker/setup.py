from setuptools import setup
import os, glob

package_name = 'kf_hungarian_tracker'

setup(
    name=package_name,
    version='0.0.1',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        ('share/' + package_name + '/config', glob.glob('config/*.yaml')),
        ('share/' + package_name + '/launch', glob.glob('launch/*.launch.py'))
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Shengjian Chen, Steven Macenski, Alexander Yuen',
    maintainer_email='csj15thu@gmail.com, stevenmacenski@gmail.com, alex@polymathrobotics.com',
    description='Use Kalman Filter and Hungarian algorithm to track multiple objects',
    license='Apache-2.0',
    tests_require=['pytest'],
    test_suite='test',  # Name of the test suite
    entry_points={
        'console_scripts': [
            'kf_hungarian_node = kf_hungarian_tracker.kf_hungarian_node:main'
        ],
    },
)

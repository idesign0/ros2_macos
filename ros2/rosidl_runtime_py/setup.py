from setuptools import find_packages
from setuptools import setup

package_name = 'rosidl_runtime_py'

setup(
    name=package_name,
    version='0.9.3',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/' + package_name, ['package.xml']),
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
    ],
    zip_safe=False,
    author='Dirk Thomas',
    author_email='dthomas@osrfoundation.org',
    maintainer='Shane Loretz',
    maintainer_email='sloretz@openrobotics.org',
    url='https://github.com/ros2/rosidl_runtime_py',
    download_url='https://github.com/ros2/rosidl_runtime_py/releases',
    keywords=[],
    classifiers=[
        'Environment :: Console',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: Apache Software License',
        'Programming Language :: Python',
    ],
    description='Runtime utilities for working with generated ROS interfaces in Python.',
    long_description=(
        'This package provides functions for operations such as populating ROS messages '
        'and converting messages to different representations.'),
    license='Apache License, Version 2.0',
    tests_require=['pytest'],
)

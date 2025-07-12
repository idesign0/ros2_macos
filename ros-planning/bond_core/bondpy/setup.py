from setuptools import find_packages
from setuptools import setup

package_name = 'bondpy'

setup(
    name=package_name,
    version='4.1.2',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Geoffrey Biggs',
    maintainer_email='geoff@openrobotics.org',
    keywords=['ROS'],
    classifiers=[
        'Intended Audience :: Developers',
        'License :: OSI Approved :: Apache Software License',
        'Programming Language :: Python',
        'Topic :: Software Development',
    ],
    description=(
        'Python implementation of bond.'
    ),
    license='BSD',
    tests_require=['pytest'],
)

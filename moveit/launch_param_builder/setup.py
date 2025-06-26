from setuptools import setup
from glob import glob

package_name = "launch_param_builder"

setup(
    name=package_name,
    version="0.1.1",
    packages=[package_name],
    data_files=[
        ("share/ament_index/resource_index/packages", ["resource/" + package_name]),
        (
            "share/" + package_name + "/launch",
            ["example/launch_param_builder_example.launch.py"],
        ),
        ("share/" + package_name + "/config", glob("test/data/*")),
        ("share/" + package_name, ["package.xml"]),
        ("lib/" + package_name, ["example/example_node.py"]),
    ],
    install_requires=["setuptools"],
    zip_safe=True,
    maintainer="Jafar Abdi",
    maintainer_email="jafar@picknik.ai",
    description="Python library for loading parameters in launch files",
    license="BSD",
    tests_require=["pytest"],
    entry_points={
        "console_scripts": [],
    },
)

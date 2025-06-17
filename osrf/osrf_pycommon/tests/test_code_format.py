import os
import subprocess
import sys


def test_flake8():
    """Test source code for pyFlakes and PEP8 conformance"""
    this_dir = os.path.dirname(os.path.abspath(__file__))
    source_dir = os.path.join(this_dir, '..', 'osrf_pycommon')
    cmd = [sys.executable, '-m', 'flake8', source_dir, '--count']
    # if flake8_import_order is installed, set the style to google
    try:
        import flake8_import_order  # noqa
        cmd.extend(['--import-order-style=google'])
    except ImportError:
        pass
    # ignore error codes from plugins this package doesn't comply with
    cmd.extend(['--ignore=C,D,Q,I'])
    # work around for https://gitlab.com/pycqa/flake8/issues/179
    cmd.extend(['--jobs', '1'])
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    stdout, stderr = p.communicate()
    print(stdout)
    assert p.returncode == 0, \
        "Command '{0}' returned non-zero exit code '{1}'".format(' '.join(cmd), p.returncode)

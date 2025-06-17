osrf_pycommon
=============

Commonly needed Python modules, used by Python software developed at OSRF.

Branches
========

If you are releasing (using ``stdeb`` or on the ROS buildfarm) for any Ubuntu < ``focal``, or for any OS that doesn't have a key for ``python3-importlib-metadata``, then you need to use the ``1.0.x`` branch, or the latest ``1.<something>`` branch, because starting with ``2.0.0``, that dependency will be required.

If you are using Python 2, then you should use the ``python2`` branch.

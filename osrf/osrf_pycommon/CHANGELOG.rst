2.1.6 (2025-03-25)
------------------
* Merge pull request `#103 <https://github.com/osrf/osrf_pycommon/issues/103>`_ from christophebedard/christophebedard/fix-typo-on-each-verb
* Contributors: Christophe Bedard

2.1.5 (2024-12-18)
------------------
* Align stdeb dependencies with setup.py (`#101 <https://github.com/osrf/osrf_pycommon/issues/101>`_)
  Follow-up to 4b2f3a8e4969f33dced1dc2db2296230e7a55b1d
* Add '+upstream' suffix to published deb version (`#102 <https://github.com/osrf/osrf_pycommon/issues/102>`_)
  Using a debian version suffix which falls late alphabetically appears to
  give our packages preference by apt. If a user enables a repository
  which distributes packages created by OSRF or ROS, it is likely that
  they wish to use these packages instead of the ones packaged by their
  platform.
* Upload coverage results to codecov (`#100 <https://github.com/osrf/osrf_pycommon/issues/100>`_)
* Update ci.yaml (`#96 <https://github.com/osrf/osrf_pycommon/issues/96>`_)
  fix node.js <20 deprecation
  Co-authored-by: Scott K Logan <logans@cottsay.net>
* Updated python version (`#97 <https://github.com/osrf/osrf_pycommon/issues/97>`_)
  Python version 3.7 is no longer supported as of June 27, 2023
  Co-authored-by: Scott K Logan <logans@cottsay.net>
* Resolve outstanding resource warnings when running tests (`#99 <https://github.com/osrf/osrf_pycommon/issues/99>`_)
* Update deb platforms for release (`#95 <https://github.com/osrf/osrf_pycommon/issues/95>`_)
  Added:
  * Ubuntu Noble (24.04 LTS pre-release)
  * Debian Trixie (testing)
  Dropped:
  * Debian Bullseye (oldstable)
  Retained:
  * Debian Bookworm (stable)
  * Ubuntu Focal (20.04 LTS)
  * Ubuntu Jammy (22.04 LTS)
* Remove CODEOWNERS. (`#98 <https://github.com/osrf/osrf_pycommon/issues/98>`_)
  It is out of date and no longer serving its intended purpose.
* Contributors: Chris Lalancette, Scott K Logan, Steven! Ragnarök, mosfet80

2.1.4 (2023-08-21)
------------------
* Catch all of the spurious warnings from get_event_loop. (`#94 <https://github.com/osrf/osrf_pycommon/issues/94>`_)
* Contributors: Chris Lalancette

2.1.3 (2023-07-11)
------------------
* Add bookworm as a python3 target (`#91 <https://github.com/osrf/osrf_pycommon/issues/91>`_)
* Suppress warning for specifically handled behavior (`#87 <https://github.com/osrf/osrf_pycommon/issues/87>`_)
* Update supported platforms (`#93 <https://github.com/osrf/osrf_pycommon/issues/93>`_)
* Add GitHub Actions CI workflow (`#88 <https://github.com/osrf/osrf_pycommon/issues/88>`_)
* Contributors: Scott K Logan, Tully Foote

2.1.2 (2023-02-14)
------------------
* [master] Update maintainers - 2022-11-07 (`#89 <https://github.com/osrf/osrf_pycommon/issues/89>`_)
* Contributors: Audrow Nash

2.1.1 (2022-11-07)
------------------
* Declare test dependencies in [test] extra (`#86 <https://github.com/osrf/osrf_pycommon/issues/86>`_)
* Contributors: Scott K Logan

2.1.0 (2022-05-10)
------------------

2.0.2 (2022-04-08)
------------------
* Fix an importlib_metadata warning with Python 3.10. (`#84 <https://github.com/osrf/osrf_pycommon/issues/84>`_)
* Contributors: Chris Lalancette

2.0.1 (2022-02-14)
------------------
* Don't release 2.x / master on Debian Buster. (`#83 <https://github.com/osrf/osrf_pycommon/issues/83>`_)
  Debian Buster is on Python 3.7: https://packages.debian.org/buster/python3
* Stop using mock in favor of unittest.mock. (`#74 <https://github.com/osrf/osrf_pycommon/issues/74>`_)
  Mock has been deprecated since Python 3.3; see
  https://pypi.org/project/mock/ .  The recommended replacement
  is unittest.mock, which seems to be a drop-in replacement.
  Co-authored-by: William Woodall <william@osrfoundation.org>
* Fix dependencies (`#81 <https://github.com/osrf/osrf_pycommon/issues/81>`_)
  * Remove obsolete setuptools from install_requires
  Now that pkg_resources are no longer used, there is no need to depend
  on setuptools at runtime.
  * Fix version-conditional dependency on importlib-metadata
  Use version markers to depend on importlib-metadata correctly.  Explicit
  conditions mean that wheels built with setup.py will either have the dep
  or not depending on what Python version they're built with, rather than
  what version they're installed on.
* fix whitespace and date in changelog heading
* Contributors: Chris Lalancette, Michał Górny, Steven! Ragnarök, William Woodall

2.0.0 (2022-02-01)
------------------
* Replace the use of ``pkg_resources`` with the more modern ``importlib-metadata``. (`#66 <https://github.com/osrf/osrf_pycommon/issues/66>`_)
  * Note this means that from now on you can only release on >= Ubuntu focal as that was when ``python3-importlib-metadata`` was introduced.
  * Used the ``1.0.x`` branch if you need an ealier version that still uses ``pkg_resources``.
  Co-authored-by: William Woodall <william@osrfoundation.org>
* Contributors: Chris Lalancette

1.0.1 (2022-01-20)
------------------
* Update release distributions. (`#78 <https://github.com/osrf/osrf_pycommon/issues/78>`_)
* Contributors: Steven! Ragnarök

1.0.0 (2021-01-25)
------------------
* Added missing conflict rules in stdeb.cfg.
* Removed Python 2 support.
* Contributors: Chris Lalancette, Timon Engelke

0.2.1 (2021-01-25)
------------------
* Fix osrf.py_common.process_utils.get_loop() implementation (`#70 <https://github.com/osrf/osrf_pycommon/issues/70>`_)
* Contributors: Michel Hidalgo

0.2.0 (2020-12-07)
------------------
* Python 2/3 version conflict (`#69 <https://github.com/osrf/osrf_pycommon/issues/69>`_)
* remove jessie because we no longer support 3.4 (`#67 <https://github.com/osrf/osrf_pycommon/issues/67>`_)
* Remove deprecated use of asyncio.coroutine decorator. (`#64 <https://github.com/osrf/osrf_pycommon/issues/64>`_)
* Fix the __str_\_ method for windows terminal_color. (`#65 <https://github.com/osrf/osrf_pycommon/issues/65>`_)
* Contributors: Chris Lalancette, Jochen Sprickerhof, William Woodall

0.1.10 (2020-05-08)
-------------------
* fixed simple deprecation warnings (issue `#61 <https://github.com/osrf/osrf_pycommon/issues/61>`_) (`#63 <https://github.com/osrf/osrf_pycommon/issues/63>`_)
* Also run tests with Python 3.7 and 3.8 (`#60 <https://github.com/osrf/osrf_pycommon/issues/60>`_)
* Remove old py2 platforms, add Suite3 option with Ubuntu Focal (`#58 <https://github.com/osrf/osrf_pycommon/issues/58>`_)
* Contributors: Shane Loretz, Zahi Kakish

0.1.9 (2019-10-10 12:55:00 -0800)
---------------------------------
* install resource marker file for package (`#56 <https://github.com/osrf/osrf_pycommon/pull/56>`_)

0.1.8 (2019-09-17 11:30:00 -0800)
---------------------------------
* Install package manifest. (`#55 <https://github.com/osrf/osrf_pycommon/issues/55>`_)
  Signed-off-by: Dirk Thomas <dirk-thomas@users.noreply.github.com>
* Rename ansi_escape_senquences to ansi_escape_sequences keeping backwards compatibility. (`#53 <https://github.com/osrf/osrf_pycommon/issues/53>`_)
* Contributors: Chris Lalancette, Dirk Thomas

0.1.7 (2019-04-11 12:45:00 -0800)
---------------------------------
* Use keyword arguments only for protocol_class invocations (`#52 <https://github.com/osrf/osrf_pycommon/issues/52>`_)
* Contributors: Daniel Stonier

0.1.6 (2018-11-15 12:45:00 -0800)
---------------------------------
- Changed package.xml to use python2 or python3 dependencies as appropriate. `#50 <https://github.com/osrf/osrf_pycommon/pull/50>`_

0.1.5 (2018-06-19 21:00:00 -0800)
---------------------------------
- Fixed a try-catch statement to adapt to changes in asyncio's raise behavior in `asyncio.get_event_loop()`.
- Small changes, mostly related to distribution.

0.1.4 (2017-12-08 16:00:00 -0800)
---------------------------------
- Only small test/linter fixes and documentation typos removed.

0.1.3 (2017-03-28 19:30:00 -0800)
---------------------------------
- Fix to support optional arguments in verb pattern `#24 <https://github.com/osrf/osrf_pycommon/pull/24>`_


0.1.2 (2016-03-28 19:30:00 -0800)
---------------------------------
- Started keeping a changelog.
- Changed ``process_utils`` module so that it will use Trollius even on Python >= 3.4 if ``trollius`` has previously been imported.

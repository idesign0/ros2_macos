^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ros2bag
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.15.14 (2025-03-25)
--------------------
* CLI - play verb metavar (backport `#1906 <https://github.com/ros2/rosbag2/issues/1906>`_) (`#1912 <https://github.com/ros2/rosbag2/issues/1912>`_)
* Contributors: mergify[bot]

0.15.13 (2024-11-25)
--------------------
* Prevent using message compression mode with mcap storage (`#1782 <https://github.com/ros2/rosbag2/issues/1782>`_)
* Contributors: Roman Shtylman

0.15.12 (2024-07-28)
--------------------
* [humble] Add option to prevent message loss while converting (backport `#1058 <https://github.com/ros2/rosbag2/issues/1058>`_) (`#1749 <https://github.com/ros2/rosbag2/issues/1749>`_)
* [humble] rosbag2_cpp: test more than one storage plugin (backport `#1196 <https://github.com/ros2/rosbag2/issues/1196>`_) (`#1721 <https://github.com/ros2/rosbag2/issues/1721>`_)
* [humble] rosbag2_storage: expose default storage ID as method (backport `#1146 <https://github.com/ros2/rosbag2/issues/1146>`_) (`#1724 <https://github.com/ros2/rosbag2/issues/1724>`_)
* [humble] Allow to specify start offset from CLI arguments equal to 0.0 for the rosbag2 player (backport `#1682 <https://github.com/ros2/rosbag2/issues/1682>`_) (`#1715 <https://github.com/ros2/rosbag2/issues/1715>`_)
* Contributors: mergify[bot]

0.15.11 (2024-05-20)
--------------------

0.15.10 (2024-05-17)
--------------------
* [humble] Add --log-level to ros2 bag play and record (`#1655 <https://github.com/ros2/rosbag2/issues/1655>`_)
* [Humble] Resolve recording option problem (backport `#1649 <https://github.com/ros2/rosbag2/issues/1649>`_) (`#1651 <https://github.com/ros2/rosbag2/issues/1651>`_)
* Contributors: Roman, mergify[bot]

0.15.9 (2024-01-24)
-------------------

0.15.8 (2023-09-19)
-------------------

0.15.7 (2023-07-18)
-------------------
* [humble] When using sim time, wait for /clock before beginning recording (backport `#1378 <https://github.com/ros2/rosbag2/issues/1378>`_) (`#1392 <https://github.com/ros2/rosbag2/issues/1392>`_)
* Contributors: Michael Orlov, mergify[bot]

0.15.6 (2023-06-05)
-------------------

0.15.5 (2023-04-25)
-------------------
* Enable document generation using rosdoc2 for ament_python pkgs (`#1260 <https://github.com/ros2/rosbag2/issues/1260>`_) (`#1261 <https://github.com/ros2/rosbag2/issues/1261>`_)
* Add Michael Orlov as maintainer in rosbag2 packages (`#1215 <https://github.com/ros2/rosbag2/issues/1215>`_) (`#1224 <https://github.com/ros2/rosbag2/issues/1224>`_)
* Contributors: mergify[bot]

0.15.4 (2023-01-10)
-------------------
* ros2bag: move storage preset validation to sqlite3 plugin (`#1135 <https://github.com/ros2/rosbag2/issues/1135>`_) (`#1184 <https://github.com/ros2/rosbag2/issues/1184>`_)
* Contributors: Daisuke Nishimatsu

0.15.3 (2022-11-07)
-------------------
* Fix for ros2 bag play exit with non-zero code on SIGINT (`#1126 <https://github.com/ros2/rosbag2/issues/1126>`_) (`#1147 <https://github.com/ros2/rosbag2/issues/1147>`_)
* Readers/info can accept a single bag storage file, and detect its storage id automatically (`#1072 <https://github.com/ros2/rosbag2/issues/1072>`_) (`#1077 <https://github.com/ros2/rosbag2/issues/1077>`_)
* Revert "[humble] Backport. Added support for filtering topics via regular expressions (`#1034 <https://github.com/ros2/rosbag2/issues/1034>`_)- (`#1039 <https://github.com/ros2/rosbag2/issues/1039>`_)" (`#1069 <https://github.com/ros2/rosbag2/issues/1069>`_)
* [humble] Backport. Added support for filtering topics via regular expressions (`#1034 <https://github.com/ros2/rosbag2/issues/1034>`_)- (`#1039 <https://github.com/ros2/rosbag2/issues/1039>`_)
* Backport. Add use_sim_time option to record verb (`#1017 <https://github.com/ros2/rosbag2/issues/1017>`_)
* Make unpublished topics unrecorded by default (`#968 <https://github.com/ros2/rosbag2/issues/968>`_) (`#1008 <https://github.com/ros2/rosbag2/issues/1008>`_)
* Contributors: Esteve Fernandez, Keisuke Shima, Sean Kelly, mergify[bot]

0.15.2 (2022-05-11)
-------------------

0.15.1 (2022-04-06)
-------------------
* support to publish as loaned message (`#981 <https://github.com/ros2/rosbag2/issues/981>`_)
* Revert "Add the ability to record any key/value pair in the 'custom' field in metadata.yaml (`#976 <https://github.com/ros2/rosbag2/issues/976>`_)" (`#984 <https://github.com/ros2/rosbag2/issues/984>`_)
* Add the ability to record any key/value pair in the 'custom' field in metadata.yaml (`#976 <https://github.com/ros2/rosbag2/issues/976>`_)
* Contributors: Audrow Nash, Barry Xu, Jorge Perez, Tony Peng

0.15.0 (2022-04-05)
-------------------
* support to publish as loaned message (`#981 <https://github.com/ros2/rosbag2/issues/981>`_)
* Revert "Add the ability to record any key/value pair in the 'custom' field in metadata.yaml (`#976 <https://github.com/ros2/rosbag2/issues/976>`_)" (`#984 <https://github.com/ros2/rosbag2/issues/984>`_)
* Add the ability to record any key/value pair in the 'custom' field in metadata.yaml (`#976 <https://github.com/ros2/rosbag2/issues/976>`_)
* Contributors: Barry Xu, Jorge Perez, Tony Peng

0.14.1 (2022-03-29)
-------------------
* Bump version number to avoid conflict
* Contributors: Chris Lalancette

0.14.0 (2022-03-29)
-------------------
* Make sure published messages are acknowledged for play mode (`#951 <https://github.com/ros2/rosbag2/issues/951>`_)
* Contributors: Barry Xu

0.13.0 (2022-01-13)
-------------------

0.12.0 (2021-12-17)
-------------------
* TopicFilter use regex_search instead of regex_match (`#932 <https://github.com/ros2/rosbag2/issues/932>`_)
* Add start-offset play option (`#931 <https://github.com/ros2/rosbag2/issues/931>`_)
* Expose bag_rewrite as `ros2 bag convert` (`#921 <https://github.com/ros2/rosbag2/issues/921>`_)
* Add "ignore leaf topics" option to recorder (`#925 <https://github.com/ros2/rosbag2/issues/925>`_)
* Auto-detect storage_id for Reader (if possible) (`#918 <https://github.com/ros2/rosbag2/issues/918>`_)
* Add pause/resume options to the bag recorder (`#905 <https://github.com/ros2/rosbag2/issues/905>`_)
* Contributors: Abrar Rahman Protyasha, Emerson Knapp, Ivan Santiago Paunovic

0.11.0 (2021-11-08)
-------------------
* Add --start-paused option to `ros2 bag play` (`#904 <https://github.com/ros2/rosbag2/issues/904>`_)
* Update package maintainers (`#899 <https://github.com/ros2/rosbag2/issues/899>`_)
* Fix converter plugin choices for record (`#897 <https://github.com/ros2/rosbag2/issues/897>`_)
* Contributors: Emerson Knapp, Ivan Santiago Paunovic, Michel Hidalgo

0.10.1 (2021-10-22)
-------------------

0.10.0 (2021-10-19)
-------------------
* Add missing spaces to error message (`#875 <https://github.com/ros2/rosbag2/issues/875>`_)
* keyboard controls for pause/resume toggle and play-next: (`#847 <https://github.com/ros2/rosbag2/issues/847>`_)
* Add --snapshot-mode argument to the "record" verb (`#851 <https://github.com/ros2/rosbag2/issues/851>`_)
* Refactor plugin query mechanism and standardize trait management (`#833 <https://github.com/ros2/rosbag2/issues/833>`_)
* Update `PlayOptions::delay` to `rclcpp::Duration` to get nanosecond resolution (`#843 <https://github.com/ros2/rosbag2/issues/843>`_)
* Load compression and serialization choices via plugin query (`#827 <https://github.com/ros2/rosbag2/issues/827>`_)
* Add delay option (`#789 <https://github.com/ros2/rosbag2/issues/789>`_)
* Avoid passing exception KeyboardInterrupt to the upper layer (`#788 <https://github.com/ros2/rosbag2/issues/788>`_)
* Contributors: Barry Xu, Cameron Miller, Emerson Knapp, Jacob Perron, Kosuke Takeuchi, Sonia Jin

0.9.0 (2021-05-17)
------------------

0.8.0 (2021-04-19)
------------------
* /clock publisher in Player (`#695 <https://github.com/ros2/rosbag2/issues/695>`_)
* Introducing Reindexer CLI (`#699 <https://github.com/ros2/rosbag2/issues/699>`_)
* rosbag2_py pybind wrapper for "record" - remove rosbag2_transport_py (`#702 <https://github.com/ros2/rosbag2/issues/702>`_)
* Add rosbag2_py::Player::play to replace rosbag2_transport_python version (`#693 <https://github.com/ros2/rosbag2/issues/693>`_)
* Explicitly add emersonknapp as maintainer (`#692 <https://github.com/ros2/rosbag2/issues/692>`_)
* Contributors: Emerson Knapp, jhdcs

0.7.0 (2021-03-18)
------------------
* use rosbag2_py for ros2 bag info (`#673 <https://github.com/ros2/rosbag2/issues/673>`_)
* CLI query rosbag2_py for available storage implementations (`#659 <https://github.com/ros2/rosbag2/issues/659>`_)
* Contributors: Emerson Knapp, Karsten Knese

0.6.0 (2021-02-01)
------------------
* Recorder --regex and --exclude options (`#604 <https://github.com/ros2/rosbag2/issues/604>`_)
* Fix the tests on cyclonedds by translating qos duration values (`#606 <https://github.com/ros2/rosbag2/issues/606>`_)
* SQLite storage optimized by default (`#568 <https://github.com/ros2/rosbag2/issues/568>`_)
* Fix a bug on parsing wrong description in plugin xml file (`#578 <https://github.com/ros2/rosbag2/issues/578>`_)
* Compress bag files in separate threads (`#506 <https://github.com/ros2/rosbag2/issues/506>`_)
* Contributors: Adam Dąbrowski, Barry Xu, Emerson Knapp, P. J. Reed

0.5.0 (2020-12-02)
------------------
* Sqlite storage double buffering (`#546 <https://github.com/ros2/rosbag2/issues/546>`_)
  * Double buffers
  * Circular queue and FLUSH option as define
  * Minor naming and lexical fixes.
  * Removed FLUSH_BUFFERS define check.
  * Sqlite3 storage logging fixes.
  * Sqlite3 storage circular buffer with pre allocated memory.
  * Sqlite3 storage buffers moved to shared_ptrs.
  * Uncrustify
  * Moved double buffers to writer
  * Buffer layer reset in seq compression writer in rosbag2 cpp
  * Buffer layer for rosbag2 writer refactor
  * Changed buffers in BufferLayer to std vectors.
  * BufferLayer uncrustify
  * Removed non-applicable test for writer cache.
  * BufferLayer review fixes
  * Rosbag metadata msgs count fixed for BufferLayer
  * Condition variable for buffer layer sync.
  * Fixed buffer locks
  * Buffers in BufferLayer refactored, moved into new class
  * Buffer layer split bags fixed.
  * Storage options include fix in buffer layer header.
  * Mutex around swapping buffers in buffer layer.
  * Fixed cache 0 bug in buffer layer.
  * Minor buffer layer refactor.
  * Counting messages in writer refactored.
  * Changed default cache size to 100Mb and updated parameter description
  * Applied review remarks:
  - significant refactoring: separation of cache classes
  - applied suggested improvements
  - some renaming
  - reduce code duplication that would otherwise increase with cache refactor, between compression and plain writers
  * Applied review comments
  - cache consumer now takes a callback and is independent of storage
  - namespace changes, renaming, cleaning
  - counting and logging messages by topic
  * linter
  * Changes after review: fixing flushing, topic counts, and more
  * Fix for splitting - flushing state now correctly turns off
  * cache classes documentation
  * simplified signature
  * a couple of tests for cache
  * address review: explicit constructor and doxygen styling
  * Windows warnings fix
  * fixed type mismatch warning on Windows
  * added minor comment
  Co-authored-by: Piotr Jaroszek <piotr.jaroszek@robotec.ai>
* Contributors: Adam Dąbrowski

0.4.0 (2020-11-19)
------------------
* read yaml config file (`#497 <https://github.com/ros2/rosbag2/issues/497>`_)
* List all storage plugins in plugin xml file (`#554 <https://github.com/ros2/rosbag2/issues/554>`_)
* add storage_config_uri (`#493 <https://github.com/ros2/rosbag2/issues/493>`_)
* Update deprecated qos policy value names (`#548 <https://github.com/ros2/rosbag2/issues/548>`_)
* Add record test for ros2bag (`#523 <https://github.com/ros2/rosbag2/issues/523>`_)
* Removed duplicated code in record (`#534 <https://github.com/ros2/rosbag2/issues/534>`_)
* Change default cache size for sequential_writer to a non zero value (`#533 <https://github.com/ros2/rosbag2/issues/533>`_)
* Update the package.xml files with the latest Open Robotics maintainers (`#535 <https://github.com/ros2/rosbag2/issues/535>`_)
* [ros2bag test_record] Gets rid of time.sleep and move to using command.wait_for_output (`#525 <https://github.com/ros2/rosbag2/issues/525>`_)
* Add pytest.ini back to ros2bag. (`#492 <https://github.com/ros2/rosbag2/issues/492>`_)
* performance testing packages (`#442 <https://github.com/ros2/rosbag2/issues/442>`_)
* Validate QoS profile values are not negative. (`#483 <https://github.com/ros2/rosbag2/issues/483>`_)
* catch parent exception (`#472 <https://github.com/ros2/rosbag2/issues/472>`_)
* add wait for closed file handles on Windows (`#470 <https://github.com/ros2/rosbag2/issues/470>`_)
* introduce ros2 bag list <plugins> (`#468 <https://github.com/ros2/rosbag2/issues/468>`_)
* move wait_for_shutdown() call out of the context manager (`#466 <https://github.com/ros2/rosbag2/issues/466>`_)
* Adding db directory creation to rosbag2_cpp (`#450 <https://github.com/ros2/rosbag2/issues/450>`_)
* use a single temp dir for the test class (`#462 <https://github.com/ros2/rosbag2/issues/462>`_)
* Add per-message ZSTD compression (`#418 <https://github.com/ros2/rosbag2/issues/418>`_)
* Add split by time to recording (`#409 <https://github.com/ros2/rosbag2/issues/409>`_)
* Add pytest.ini so local tests don't display warning (`#446 <https://github.com/ros2/rosbag2/issues/446>`_)
* Contributors: Adam Dąbrowski, Barry Xu, Chris Lalancette, Dirk Thomas, Ivan Santiago Paunovic, Jacob Perron, Jaison Titus, Jesse Ikawa, Karsten Knese, Marwan Taher, Michael Jeronimo, P. J. Reed, jhdcs

0.3.2 (2020-06-03)
------------------
* Improve help message for CLI verbs (`#427 <https://github.com/ros2/rosbag2/issues/427>`_)
* Contributors: Jacob Perron

0.3.1 (2020-06-01)
------------------

0.3.0 (2020-05-26)
------------------
* Don't allow user to specify unimplemented compression mode 'message' (`#415 <https://github.com/ros2/rosbag2/issues/415>`_)
* Use consistent quotes in help messages (`#416 <https://github.com/ros2/rosbag2/issues/416>`_)
* Contributors: Dirk Thomas, Emerson Knapp

0.2.8 (2020-05-18)
------------------

0.2.7 (2020-05-12)
------------------

0.2.6 (2020-05-07)
------------------

0.2.5 (2020-04-30)
------------------
* add topic remapping option to rosbag2 play (`#388 <https://github.com/ros2/rosbag2/issues/388>`_)
* Add loop option to rosbag play (`#361 <https://github.com/ros2/rosbag2/issues/361>`_)
* Expose topic filter to command line (addresses `#342 <https://github.com/ros2/rosbag2/issues/342>`_) (`#363 <https://github.com/ros2/rosbag2/issues/363>`_)
* Override QoS Profiles in CLI - Playback (`#356 <https://github.com/ros2/rosbag2/issues/356>`_)
* Refactor utility functions in ros2bag (`#358 <https://github.com/ros2/rosbag2/issues/358>`_)
* Add QoS Profile override to CLI (`#347 <https://github.com/ros2/rosbag2/issues/347>`_)
* Transaction based sqlite3 inserts (`#225 <https://github.com/ros2/rosbag2/issues/225>`_)
* include hidden topics (`#332 <https://github.com/ros2/rosbag2/issues/332>`_)
* more verbose test_flake8 error messages (same as `ros2/launch_ros#135 <https://github.com/ros2/launch_ros/issues/135>`_)
* Add playback rate command line arg (`#304 <https://github.com/ros2/rosbag2/issues/304>`_)
* [compression] Enable compression through ros2bag cli (`#263 <https://github.com/ros2/rosbag2/issues/263>`_)
* switch to not deprecated API (`#261 <https://github.com/ros2/rosbag2/issues/261>`_)
* make ros tooling working group maintainer (`#211 <https://github.com/ros2/rosbag2/issues/211>`_)
* Contributors: Anas Abou Allaban, Dirk Thomas, Karsten Knese, Mabel Zhang, Sriram Raghunathan, Zachary Michaels, ketatam

0.2.4 (2019-11-18)
------------------

0.2.3 (2019-11-18)
------------------
* Add CLI option to expose option for bagfile splitting (`#203 <https://github.com/ros2/rosbag2/issues/203>`_)
* Contributors: Karsten Knese, Prajakta Gokhale

0.2.2 (2019-11-13)
------------------

0.2.1 (2019-10-23)
------------------
* Fix flake8 errors and add missing lint tests. (`#194 <https://github.com/ros2/rosbag2/issues/194>`_)
* Import rosbag2_transport Python module on demand. (`#190 <https://github.com/ros2/rosbag2/issues/190>`_)
* Contributors: Michel Hidalgo, Thomas Moulard

0.2.0 (2019-09-26)
------------------
* install resource marker file for package (`#167 <https://github.com/ros2/rosbag2/issues/167>`_)
* install package manifest (`#161 <https://github.com/ros2/rosbag2/issues/161>`_)
* Contributors: Dirk Thomas, Ruffin

0.1.2 (2019-05-20)
------------------
* remove disclaimer (`#122 <https://github.com/ros2/rosbag2/issues/122>`_)
  Signed-off-by: Karsten Knese <karsten@openrobotics.org>
* Contributors: Karsten Knese

0.1.1 (2019-05-09)
------------------

0.1.0 (2019-05-08)
------------------
* Fix issue with ros2bag record python frontend (`#100 <https://github.com/ros2/rosbag2/issues/100>`_)
* Consistent node naming across ros2cli tools (`#60 <https://github.com/ros2/rosbag2/issues/60>`_)
* Contributors: AAlon, Sagnik Basu

0.0.5 (2018-12-27)
------------------

0.0.4 (2018-12-19)
------------------
* 0.0.3
* Play old bagfiles (`#69 <https://github.com/bsinno/rosbag2/issues/69>`_)
* Release fixups (`#72 <https://github.com/bsinno/rosbag2/issues/72>`_)
* Contributors: Andreas Holzner, Karsten Knese, Martin Idel

0.0.2 (2018-12-12)
------------------
* update maintainer email
* Contributors: Karsten Knese

0.0.1 (2018-12-11)
------------------
* Auto discovery of new topics (`#63 <https://github.com/ros2/rosbag2/issues/63>`_)
* Use converters when recording a bag file (`#57 <https://github.com/ros2/rosbag2/issues/57>`_)
* Display bag summary using `ros2 bag info` (`#45 <https://github.com/ros2/rosbag2/issues/45>`_)
* Use directory as bagfile and add additonal record options (`#43 <https://github.com/ros2/rosbag2/issues/43>`_)
* Introduce rosbag2_transport layer and CLI (`#38 <https://github.com/ros2/rosbag2/issues/38>`_)
* initial command line interface (`#12 <https://github.com/ros2/rosbag2/issues/12>`_)
* (demo, sqlite3) First working rosbag2 implementation (`#6 <https://github.com/ros2/rosbag2/issues/6>`_)
* initial setup
* Contributors: Alessandro Bottero, Andreas Greimel, Karsten Knese, Martin Idel

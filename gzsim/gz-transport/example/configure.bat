@rem Run CMake, pointing to sibling directories containing dependencies.
@rem Note that zmq and cppzmq are relative to the source dir, while
@rem protobuf is relative to the build dir.  Not sure why.
@set build_type=Release
@if not "%1"=="" set build_type=%1
@set build_bitness=64
@if not "%2"=="" set build_bitness=%2
@echo Configuring for build type %build_type%

@set PROTOBUF_PATH=%cd%\..\..\..\protobuf-2.6.0-win%build_bitness%-vc12
@set IGN_TRANSPORT_PATH=%cd%\..\..\build\install\%build_type%
@set IGNITION-MSGS_PATH=%cd%\..\..\..\ign-msgs\build\install\%build_type%
@set IGNITION-MSGS_CMAKE_PREFIX_PATH=%IGNITION-MSGS_PATH%\CMake
@set IGNITION-MATH_PATH=%cd%\..\..\..\ign-math\build\install\%build_type%
@set IGNITION-MATH_CMAKE_PREFIX_PATH=%IGNITION-MATH_PATH%\CMake

cmake -G "NMake Makefiles"^
      -DCMAKE_PREFIX_PATH="%IGN_TRANSPORT_PATH%\lib\cmake\ignition-transport11;%IGNITION-MSGS_CMAKE_PREFIX_PATH%;%IGNITION-MATH_CMAKE_PREFIX_PATH%;"^
      -DPROTOBUF_SRC_ROOT_FOLDER="%PROTOBUF_PATH%"^
      -DIGNITION-MSGS_FOLDER="%IGNITION-MSGS_PATH%"^
      -DCMAKE_INSTALL_PREFIX="install"^
      -DCMAKE_BUILD_TYPE=%build_type%^
      ..

@if %errorlevel% neq 0 exit /b %errorlevel%
@echo Configuration complete.  To build, run `nmake`

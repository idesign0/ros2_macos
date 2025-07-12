
READ ME 
This document contains the steps to execute few tests.
1. Build the package
colcon build

2. Do the test
colcon test

3. The executable are generated follow the below steps to run tests.

3.1 source local setup 
source install/local_setup.sh

3.2 run the executables.
./install/test_bond/lib/test_bond/exercise_bond_cpp_exc

./install/test_bond/lib/test_bond/test_callbacks_cpp_exc

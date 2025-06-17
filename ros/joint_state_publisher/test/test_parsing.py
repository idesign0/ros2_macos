# Copyright 2023 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import math

import joint_state_publisher.joint_state_publisher
import pytest
import rclpy


@pytest.fixture
def rclpy_fixture():
    rclpy.init()
    yield
    rclpy.try_shutdown()


def test_urdf_continuous(tmpdir, rclpy_fixture):
    urdf = """<?xml version="1.0"?>
<urdf>
  <robot name="multi_joint_robot">
    <link name="link1"/>
    <link name="link2"/>

    <joint name="j12" type="continuous">
      <parent link="link1"/>
      <child link="link2"/>
      <limit effort="10" velocity="10" lower="-1" upper="1"/>
    </joint>
  </robot>
</urdf>"""

    urdf_filename = tmpdir / 'continuous.urdf'
    urdf_filename.write_text(urdf, encoding='utf-8')

    jsp = joint_state_publisher.joint_state_publisher.JointStatePublisher(urdf_filename)

    assert len(jsp.free_joints) == 1
    assert jsp.free_joints['j12']['continuous']
    assert jsp.free_joints['j12']['max'] == pytest.approx(math.pi)
    assert jsp.free_joints['j12']['min'] == pytest.approx(-math.pi)
    assert jsp.free_joints['j12']['position'] == 0.0
    assert jsp.free_joints['j12']['zero'] == 0.0

    assert len(jsp.joint_list) == 1
    assert jsp.joint_list[0] == 'j12'

    assert not jsp.dependent_joints


def test_urdf_revolute(tmpdir, rclpy_fixture):
    urdf = """<?xml version="1.0"?>
<urdf>
  <robot name="multi_joint_robot">
    <link name="link1"/>
    <link name="link2"/>

    <joint name="j12" type="revolute">
      <parent link="link1"/>
      <child link="link2"/>
      <limit effort="10" velocity="10" lower="-1" upper="1"/>
    </joint>
  </robot>
</urdf>"""

    urdf_filename = tmpdir / 'revolute.urdf'
    urdf_filename.write_text(urdf, encoding='utf-8')

    jsp = joint_state_publisher.joint_state_publisher.JointStatePublisher(urdf_filename)

    assert len(jsp.free_joints) == 1
    assert jsp.free_joints['j12']['min'] == -1
    assert jsp.free_joints['j12']['max'] == 1
    assert jsp.free_joints['j12']['position'] == 0.0
    assert jsp.free_joints['j12']['zero'] == 0.0

    assert len(jsp.joint_list) == 1
    assert jsp.joint_list[0] == 'j12'

    assert not jsp.dependent_joints


def test_urdf_fixed(tmpdir, rclpy_fixture):
    urdf = """<?xml version="1.0"?>
<urdf>
  <robot name="multi_joint_robot">
    <link name="link1"/>
    <link name="link2"/>

    <joint name="j12" type="fixed">
      <parent link="link1"/>
      <child link="link2"/>
    </joint>
  </robot>
</urdf>"""

    urdf_filename = tmpdir / 'fixed.urdf'
    urdf_filename.write_text(urdf, encoding='utf-8')

    jsp = joint_state_publisher.joint_state_publisher.JointStatePublisher(urdf_filename)

    assert not jsp.free_joints

    assert not jsp.joint_list

    assert not jsp.dependent_joints


def test_urdf_revolute_without_limit(tmpdir, rclpy_fixture):
    urdf = """<?xml version="1.0"?>
<urdf>
  <robot name="multi_joint_robot">
    <link name="link1"/>
    <link name="link2"/>

    <joint name="j12" type="revolute">
      <parent link="link1"/>
      <child link="link2"/>
    </joint>
  </robot>
</urdf>"""

    urdf_filename = tmpdir / 'revolute.urdf'
    urdf_filename.write_text(urdf, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(urdf_filename)
    assert str(excinfo.value) == 'Limits must be specified for joint "j12" of type "revolute"'


def test_urdf_revolute_without_lower(tmpdir, rclpy_fixture):
    urdf = """<?xml version="1.0"?>
<urdf>
  <robot name="multi_joint_robot">
    <link name="link1"/>
    <link name="link2"/>

    <joint name="j12" type="revolute">
      <parent link="link1"/>
      <child link="link2"/>
      <limit effort="10" velocity="10" upper="1"/>
    </joint>
  </robot>
</urdf>"""

    urdf_filename = tmpdir / 'revolute.urdf'
    urdf_filename.write_text(urdf, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(urdf_filename)
    assert \
        str(excinfo.value) == '"lower" limit must be specified for joint "j12" of type "revolute"'


def test_urdf_revolute_without_upper(tmpdir, rclpy_fixture):
    urdf = """<?xml version="1.0"?>
<urdf>
  <robot name="multi_joint_robot">
    <link name="link1"/>
    <link name="link2"/>

    <joint name="j12" type="revolute">
      <parent link="link1"/>
      <child link="link2"/>
      <limit effort="10" velocity="10" lower="-1"/>
    </joint>
  </robot>
</urdf>"""

    urdf_filename = tmpdir / 'revolute.urdf'
    urdf_filename.write_text(urdf, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(urdf_filename)
    assert \
        str(excinfo.value) == '"upper" limit must be specified for joint "j12" of type "revolute"'


def test_urdf_revolute_with_bogus_lower(tmpdir, rclpy_fixture):
    urdf = """<?xml version="1.0"?>
<urdf>
  <robot name="multi_joint_robot">
    <link name="link1"/>
    <link name="link2"/>

    <joint name="j12" type="revolute">
      <parent link="link1"/>
      <child link="link2"/>
      <limit effort="10" velocity="10" lower="foo" upper="1"/>
    </joint>
  </robot>
</urdf>"""

    urdf_filename = tmpdir / 'revolute.urdf'
    urdf_filename.write_text(urdf, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(urdf_filename)
    assert \
        str(excinfo.value) == '"lower" limit must be a float for joint "j12" of type "revolute"'


def test_urdf_revolute_with_bogus_upper(tmpdir, rclpy_fixture):
    urdf = """<?xml version="1.0"?>
<urdf>
  <robot name="multi_joint_robot">
    <link name="link1"/>
    <link name="link2"/>

    <joint name="j12" type="revolute">
      <parent link="link1"/>
      <child link="link2"/>
      <limit effort="10" velocity="10" lower="-1" upper="foo"/>
    </joint>
  </robot>
</urdf>"""

    urdf_filename = tmpdir / 'revolute.urdf'
    urdf_filename.write_text(urdf, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(urdf_filename)
    assert \
        str(excinfo.value) == '"upper" limit must be a float for joint "j12" of type "revolute"'


def test_urdf_without_robot(tmpdir, rclpy_fixture):
    urdf = """<?xml version="1.0"?>
<urdf>
    <link name="link1"/>
    <link name="link2"/>

    <joint name="j12" type="revolute">
      <parent link="link1"/>
      <child link="link2"/>
      <limit effort="10" velocity="10" lower="-1" upper="1"/>
    </joint>
</urdf>"""

    urdf_filename = tmpdir / 'revolute.urdf'
    urdf_filename.write_text(urdf, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(urdf_filename)
    assert str(excinfo.value) == 'URDF must have a "robot" tag'


def test_collada_revolute(tmpdir, rclpy_fixture):
    collada = """<?xml version="1.0" encoding="UTF-8"?>
<COLLADA xmlns="http://www.collada.org/2008/03/COLLADASchema" version="1.5.0">
  <library_kinematics_models id="kmodels">
    <kinematics_model id="kmodel0" name="multi_joint_robot">
      <technique_common>
        <joint name="j12" sid="j12">
          <revolute sid="axis0">
            <axis>1 0 0</axis>
            <limits>
              <min>-57.29577951308232</min>
              <max>57.29577951308232</max>
            </limits>
          </revolute>
        </joint>
      </technique_common>
    </kinematics_model>
  </library_kinematics_models>
</COLLADA>"""

    collada_filename = tmpdir / 'revolute.dae'
    collada_filename.write_text(collada, encoding='utf-8')

    jsp = joint_state_publisher.joint_state_publisher.JointStatePublisher(collada_filename)

    assert len(jsp.free_joints) == 1
    assert jsp.free_joints['j12']['max'] == pytest.approx(1.0)
    assert jsp.free_joints['j12']['min'] == pytest.approx(-1.0)
    assert jsp.free_joints['j12']['position'] == 0.0
    assert jsp.free_joints['j12']['zero'] == 0.0

    assert len(jsp.joint_list) == 1
    assert jsp.joint_list[0] == 'j12'

    assert not jsp.dependent_joints


def test_collada_fixed(tmpdir, rclpy_fixture):
    collada = """<?xml version="1.0" encoding="UTF-8"?>
<COLLADA xmlns="http://www.collada.org/2008/03/COLLADASchema" version="1.5.0">
  <library_kinematics_models id="kmodels">
    <kinematics_model id="kmodel0" name="multi_joint_robot">
      <technique_common>
        <joint name="j12" sid="j12">
          <revolute sid="axis0">
            <axis>1 0 0</axis>
            <limits>
              <min>57.29577951308232</min>
              <max>57.29577951308232</max>
            </limits>
          </revolute>
        </joint>
      </technique_common>
    </kinematics_model>
  </library_kinematics_models>
</COLLADA>"""

    collada_filename = tmpdir / 'fixed.dae'
    collada_filename.write_text(collada, encoding='utf-8')

    jsp = joint_state_publisher.joint_state_publisher.JointStatePublisher(collada_filename)

    assert not jsp.free_joints

    assert not jsp.joint_list

    assert not jsp.dependent_joints


def test_collada_no_revolute(tmpdir, rclpy_fixture):
    collada = """<?xml version="1.0" encoding="UTF-8"?>
<COLLADA xmlns="http://www.collada.org/2008/03/COLLADASchema" version="1.5.0">
  <library_kinematics_models id="kmodels">
    <kinematics_model id="kmodel0" name="multi_joint_robot">
      <technique_common>
        <joint name="j12" sid="j12">
        </joint>
      </technique_common>
    </kinematics_model>
  </library_kinematics_models>
</COLLADA>"""

    collada_filename = tmpdir / 'fixed.dae'
    collada_filename.write_text(collada, encoding='utf-8')

    jsp = joint_state_publisher.joint_state_publisher.JointStatePublisher(collada_filename)

    assert not jsp.free_joints

    assert not jsp.joint_list

    assert not jsp.dependent_joints


def test_collada_revolute_without_limits(tmpdir, rclpy_fixture):
    collada = """<?xml version="1.0" encoding="UTF-8"?>
<COLLADA xmlns="http://www.collada.org/2008/03/COLLADASchema" version="1.5.0">
  <library_kinematics_models id="kmodels">
    <kinematics_model id="kmodel0" name="multi_joint_robot">
      <technique_common>
        <joint name="j12" sid="j12">
          <revolute sid="axis0">
            <axis>1 0 0</axis>
          </revolute>
        </joint>
      </technique_common>
    </kinematics_model>
  </library_kinematics_models>
</COLLADA>"""

    collada_filename = tmpdir / 'revolute.dae'
    collada_filename.write_text(collada, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(collada_filename)
    assert str(excinfo.value) == 'Limits must be specified for joint "j12" of type "revolute"'


def test_collada_revolute_without_min(tmpdir, rclpy_fixture):
    collada = """<?xml version="1.0" encoding="UTF-8"?>
<COLLADA xmlns="http://www.collada.org/2008/03/COLLADASchema" version="1.5.0">
  <library_kinematics_models id="kmodels">
    <kinematics_model id="kmodel0" name="multi_joint_robot">
      <technique_common>
        <joint name="j12" sid="j12">
          <revolute sid="axis0">
            <axis>1 0 0</axis>
            <limits>
              <max>57.29577951308232</max>
            </limits>
          </revolute>
        </joint>
      </technique_common>
    </kinematics_model>
  </library_kinematics_models>
</COLLADA>"""

    collada_filename = tmpdir / 'revolute.dae'
    collada_filename.write_text(collada, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(collada_filename)
    assert \
        str(excinfo.value) == '"min" limit must be specified for joint "j12" of type "revolute"'


def test_collada_revolute_without_max(tmpdir, rclpy_fixture):
    collada = """<?xml version="1.0" encoding="UTF-8"?>
<COLLADA xmlns="http://www.collada.org/2008/03/COLLADASchema" version="1.5.0">
  <library_kinematics_models id="kmodels">
    <kinematics_model id="kmodel0" name="multi_joint_robot">
      <technique_common>
        <joint name="j12" sid="j12">
          <revolute sid="axis0">
            <axis>1 0 0</axis>
            <limits>
              <min>-57.29577951308232</min>
            </limits>
          </revolute>
        </joint>
      </technique_common>
    </kinematics_model>
  </library_kinematics_models>
</COLLADA>"""

    collada_filename = tmpdir / 'revolute.dae'
    collada_filename.write_text(collada, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(collada_filename)
    assert \
        str(excinfo.value) == '"max" limit must be specified for joint "j12" of type "revolute"'


def test_collada_revolute_with_bogus_min(tmpdir, rclpy_fixture):
    collada = """<?xml version="1.0" encoding="UTF-8"?>
<COLLADA xmlns="http://www.collada.org/2008/03/COLLADASchema" version="1.5.0">
  <library_kinematics_models id="kmodels">
    <kinematics_model id="kmodel0" name="multi_joint_robot">
      <technique_common>
        <joint name="j12" sid="j12">
          <revolute sid="axis0">
            <axis>1 0 0</axis>
            <limits>
              <min>foo</min>
              <max>57.29577951308232</max>
            </limits>
          </revolute>
        </joint>
      </technique_common>
    </kinematics_model>
  </library_kinematics_models>
</COLLADA>"""

    collada_filename = tmpdir / 'revolute.dae'
    collada_filename.write_text(collada, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(collada_filename)
    assert str(excinfo.value) == '"min" limit must be a float for joint "j12" of type "revolute"'


def test_collada_revolute_with_bogus_max(tmpdir, rclpy_fixture):
    collada = """<?xml version="1.0" encoding="UTF-8"?>
<COLLADA xmlns="http://www.collada.org/2008/03/COLLADASchema" version="1.5.0">
  <library_kinematics_models id="kmodels">
    <kinematics_model id="kmodel0" name="multi_joint_robot">
      <technique_common>
        <joint name="j12" sid="j12">
          <revolute sid="axis0">
            <axis>1 0 0</axis>
            <limits>
              <min>-57.29577951308232</min>
              <max>foo</max>
            </limits>
          </revolute>
        </joint>
      </technique_common>
    </kinematics_model>
  </library_kinematics_models>
</COLLADA>"""

    collada_filename = tmpdir / 'revolute.dae'
    collada_filename.write_text(collada, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(collada_filename)
    assert str(excinfo.value) == '"max" limit must be a float for joint "j12" of type "revolute"'


def test_collada_no_version_attribute(tmpdir, rclpy_fixture):
    collada = """<?xml version="1.0" encoding="UTF-8"?>
<COLLADA xmlns="http://www.collada.org/2008/03/COLLADASchema">
  <library_kinematics_models id="kmodels">
    <kinematics_model id="kmodel0" name="multi_joint_robot">
      <technique_common>
        <joint name="j12" sid="j12">
          <revolute sid="axis0">
            <axis>1 0 0</axis>
            <limits>
              <min>-57.29577951308232</min>
              <max>57.29577951308232</max>
            </limits>
          </revolute>
        </joint>
      </technique_common>
    </kinematics_model>
  </library_kinematics_models>
</COLLADA>"""

    collada_filename = tmpdir / 'revolute.dae'
    collada_filename.write_text(collada, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(collada_filename)
    assert str(excinfo.value) == 'COLLADA must have a version tag'


def test_collada_version_too_old(tmpdir, rclpy_fixture):
    collada = """<?xml version="1.0" encoding="UTF-8"?>
<COLLADA xmlns="http://www.collada.org/2008/03/COLLADASchema" version="1.4.0">
  <library_kinematics_models id="kmodels">
    <kinematics_model id="kmodel0" name="multi_joint_robot">
      <technique_common>
        <joint name="j12" sid="j12">
          <revolute sid="axis0">
            <axis>1 0 0</axis>
            <limits>
              <min>-57.29577951308232</min>
              <max>57.29577951308232</max>
            </limits>
          </revolute>
        </joint>
      </technique_common>
    </kinematics_model>
  </library_kinematics_models>
</COLLADA>"""

    collada_filename = tmpdir / 'revolute.dae'
    collada_filename.write_text(collada, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(collada_filename)
    assert str(excinfo.value) == 'COLLADA must be at least version 1.5.0'


def test_collada_without_kinematics_model(tmpdir, rclpy_fixture):
    collada = """<?xml version="1.0" encoding="UTF-8"?>
<COLLADA xmlns="http://www.collada.org/2008/03/COLLADASchema" version="1.5.0">
  <library_kinematics_models id="kmodels">
      <technique_common>
        <joint name="j12" sid="j12">
          <revolute sid="axis0">
            <axis>1 0 0</axis>
            <limits>
              <min>-57.29577951308232</min>
              <max>57.29577951308232</max>
            </limits>
          </revolute>
        </joint>
      </technique_common>
  </library_kinematics_models>
</COLLADA>"""

    collada_filename = tmpdir / 'revolute.dae'
    collada_filename.write_text(collada, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(collada_filename)
    assert str(excinfo.value) == 'COLLADA must have a "kinematics_model" tag'


def test_collada_without_technique_common(tmpdir, rclpy_fixture):
    collada = """<?xml version="1.0" encoding="UTF-8"?>
<COLLADA xmlns="http://www.collada.org/2008/03/COLLADASchema" version="1.5.0">
  <library_kinematics_models id="kmodels">
    <kinematics_model id="kmodel0" name="multi_joint_robot">
        <joint name="j12" sid="j12">
          <revolute sid="axis0">
            <axis>1 0 0</axis>
            <limits>
              <min>-57.29577951308232</min>
              <max>57.29577951308232</max>
            </limits>
          </revolute>
        </joint>
    </kinematics_model>
  </library_kinematics_models>
</COLLADA>"""

    collada_filename = tmpdir / 'revolute.dae'
    collada_filename.write_text(collada, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(collada_filename)
    assert str(excinfo.value) == 'COLLADA must have a "technique_common" tag'


def test_sdf_continuous(tmpdir, rclpy_fixture):
    sdf = """<?xml version="1.0"?>
<sdf version="1.8">
  <model name='multi_joint_robot'>
    <link name="link1"/>
    <link name="link2"/>

    <joint name='j12' type='continuous'>
      <parent>link1</parent>
      <child>link2</child>
      <axis>
        <xyz>0 0 1</xyz>
      </axis>
    </joint>
  </model>
</sdf>"""

    sdf_filename = tmpdir / 'continuous.sdf'
    sdf_filename.write_text(sdf, encoding='utf-8')

    jsp = joint_state_publisher.joint_state_publisher.JointStatePublisher(sdf_filename)

    assert len(jsp.free_joints) == 1
    assert jsp.free_joints['j12']['min'] == pytest.approx(-math.pi)
    assert jsp.free_joints['j12']['max'] == pytest.approx(math.pi)
    assert jsp.free_joints['j12']['position'] == 0.0
    assert jsp.free_joints['j12']['zero'] == 0.0

    assert len(jsp.joint_list) == 1
    assert jsp.joint_list[0] == 'j12'

    assert not jsp.dependent_joints


def test_sdf_revolute(tmpdir, rclpy_fixture):
    sdf = """<?xml version="1.0"?>
<sdf version="1.8">
  <model name='multi_joint_robot'>
    <link name="link1"/>
    <link name="link2"/>

    <joint name='j12' type='revolute'>
      <parent>link1</parent>
      <child>link2</child>
      <axis>
        <xyz>0 0 1</xyz>
        <limit>
          <lower>-1</lower>
          <upper>1</upper>
        </limit>
      </axis>
    </joint>
  </model>
</sdf>"""

    sdf_filename = tmpdir / 'revolute.sdf'
    sdf_filename.write_text(sdf, encoding='utf-8')

    jsp = joint_state_publisher.joint_state_publisher.JointStatePublisher(sdf_filename)

    assert len(jsp.free_joints) == 1
    assert jsp.free_joints['j12']['min'] == -1
    assert jsp.free_joints['j12']['max'] == 1
    assert jsp.free_joints['j12']['position'] == 0.0
    assert jsp.free_joints['j12']['zero'] == 0.0

    assert len(jsp.joint_list) == 1
    assert jsp.joint_list[0] == 'j12'

    assert not jsp.dependent_joints


def test_sdf_fixed(tmpdir, rclpy_fixture):
    sdf = """<?xml version="1.0"?>
<sdf version="1.8">
  <model name='multi_joint_robot'>
    <link name="link1"/>
    <link name="link2"/>

    <joint name='j12' type='fixed'>
      <parent>link1</parent>
      <child>link2</child>
    </joint>
  </model>
</sdf>"""

    sdf_filename = tmpdir / 'fixed.sdf'
    sdf_filename.write_text(sdf, encoding='utf-8')

    jsp = joint_state_publisher.joint_state_publisher.JointStatePublisher(sdf_filename)

    assert not jsp.free_joints

    assert not jsp.joint_list

    assert not jsp.dependent_joints


def test_sdf_revolute_without_limit(tmpdir, rclpy_fixture):
    sdf = """<?xml version="1.0"?>
<sdf version="1.8">
  <model name='multi_joint_robot'>
    <link name="link1"/>
    <link name="link2"/>

    <joint name='j12' type='revolute'>
      <parent>link1</parent>
      <child>link2</child>
      <axis>
        <xyz>0 0 1</xyz>
      </axis>
    </joint>
  </model>
</sdf>"""

    sdf_filename = tmpdir / 'revolute.sdf'
    sdf_filename.write_text(sdf, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(sdf_filename)
    assert str(excinfo.value) == 'Limits must be specified for joint "j12" of type "revolute"'


def test_sdf_revolute_without_lower(tmpdir, rclpy_fixture):
    sdf = """<?xml version="1.0"?>
<sdf version="1.8">
  <model name='multi_joint_robot'>
    <link name="link1"/>
    <link name="link2"/>

    <joint name='j12' type='revolute'>
      <parent>link1</parent>
      <child>link2</child>
      <axis>
        <xyz>0 0 1</xyz>
        <limit>
          <upper>1</upper>
        </limit>
      </axis>
    </joint>
  </model>
</sdf>"""

    sdf_filename = tmpdir / 'revolute.sdf'
    sdf_filename.write_text(sdf, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(sdf_filename)
    assert \
        str(excinfo.value) == '"lower" limit must be specified for joint "j12" of type "revolute"'


def test_sdf_revolute_without_upper(tmpdir, rclpy_fixture):
    sdf = """<?xml version="1.0"?>
<sdf version="1.8">
  <model name='multi_joint_robot'>
    <link name="link1"/>
    <link name="link2"/>

    <joint name='j12' type='revolute'>
      <parent>link1</parent>
      <child>link2</child>
      <axis>
        <xyz>0 0 1</xyz>
        <limit>
          <lower>-1</lower>
        </limit>
      </axis>
    </joint>
  </model>
</sdf>"""

    sdf_filename = tmpdir / 'revolute.sdf'
    sdf_filename.write_text(sdf, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(sdf_filename)
    assert \
        str(excinfo.value) == '"upper" limit must be specified for joint "j12" of type "revolute"'


def test_sdf_revolute_with_bogus_lower(tmpdir, rclpy_fixture):
    sdf = """<?xml version="1.0"?>
<sdf version="1.8">
  <model name='multi_joint_robot'>
    <link name="link1"/>
    <link name="link2"/>

    <joint name='j12' type='revolute'>
      <parent>link1</parent>
      <child>link2</child>
      <axis>
        <xyz>0 0 1</xyz>
        <limit>
          <lower>foo</lower>
          <upper>1</upper>
        </limit>
      </axis>
    </joint>
  </model>
</sdf>"""

    sdf_filename = tmpdir / 'revolute.sdf'
    sdf_filename.write_text(sdf, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(sdf_filename)
    assert \
        str(excinfo.value) == '"lower" limit must be a float for joint "j12" of type "revolute"'


def test_sdf_revolute_with_bogus_upper(tmpdir, rclpy_fixture):
    sdf = """<?xml version="1.0"?>
<sdf version="1.8">
  <model name='multi_joint_robot'>
    <link name="link1"/>
    <link name="link2"/>

    <joint name='j12' type='revolute'>
      <parent>link1</parent>
      <child>link2</child>
      <axis>
        <xyz>0 0 1</xyz>
        <limit>
          <lower>-1</lower>
          <upper>foo</upper>
        </limit>
      </axis>
    </joint>
  </model>
</sdf>"""

    sdf_filename = tmpdir / 'revolute.sdf'
    sdf_filename.write_text(sdf, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(sdf_filename)
    assert \
        str(excinfo.value) == '"upper" limit must be a float for joint "j12" of type "revolute"'


def test_sdf_without_model(tmpdir, rclpy_fixture):
    sdf = """<?xml version="1.0"?>
<sdf version="1.8">
    <link name="link1"/>
    <link name="link2"/>

    <joint name='j12' type='revolute'>
      <parent>link1</parent>
      <child>link2</child>
      <axis>
        <xyz>0 0 1</xyz>
        <limit>
          <lower>-1</lower>
          <upper>1</upper>
        </limit>
      </axis>
    </joint>
</sdf>"""

    sdf_filename = tmpdir / 'revolute.sdf'
    sdf_filename.write_text(sdf, encoding='utf-8')

    with pytest.raises(Exception) as excinfo:
        joint_state_publisher.joint_state_publisher.JointStatePublisher(sdf_filename)
    assert str(excinfo.value) == 'SDF must have a "model" tag'

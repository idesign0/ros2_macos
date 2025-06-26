#!/usr/bin/env python3
PKG = "srdfdom"

import sys
import os
import unittest
from srdfdom.srdf import SRDF
from xml.dom.minidom import parseString
import xml.dom

# xml match code from test_xacro.py
# by Stuart Glaser and William Woodall


def first_child_element(elt):
    c = elt.firstChild
    while c:
        if c.nodeType == xml.dom.Node.ELEMENT_NODE:
            return c
        c = c.nextSibling
    return None


def next_sibling_element(elt):
    c = elt.nextSibling
    while c:
        if c.nodeType == xml.dom.Node.ELEMENT_NODE:
            return c
        c = c.nextSibling
    return None


def all_attributes_match(a, b):
    if len(a.attributes) != len(b.attributes):
        print("Different number of attributes")
        return False
    a_atts = [
        (a.attributes.item(i).name, a.attributes.item(i).value)
        for i in range(len(a.attributes))
    ]
    b_atts = [
        (b.attributes.item(i).name, b.attributes.item(i).value)
        for i in range(len(b.attributes))
    ]
    a_atts.sort()
    b_atts.sort()
    for i in range(len(a_atts)):
        if a_atts[i][0] != b_atts[i][0]:
            print("Different attribute names: %s and %s" % (a_atts[i][0], b_atts[i][0]))
            return False
        try:
            if abs(float(a_atts[i][1]) - float(b_atts[i][1])) > 1.0e-9:
                print(
                    "Different attribute values: %s and %s"
                    % (a_atts[i][1], b_atts[i][1])
                )
                return False
        except ValueError:  # Attribute values aren't numeric
            if a_atts[i][1] != b_atts[i][1]:
                print(
                    "Different attribute values: %s and %s"
                    % (a_atts[i][1], b_atts[i][1])
                )
                return False
    return True


def elements_match(a, b):
    if not a and not b:
        return True
    if not a or not b:
        return False
    if a.nodeType != b.nodeType:
        print("Different node types: %d and %d" % (a.nodeType, b.nodeType))
        return False
    if a.nodeName != b.nodeName:
        print("Different element names: %s and %s" % (a.nodeName, b.nodeName))
        return False
    if not all_attributes_match(a, b):
        return False
    if not elements_match(first_child_element(a), first_child_element(b)):
        return False
    if not elements_match(next_sibling_element(a), next_sibling_element(b)):
        return False
    return True


def xml_matches(a, b):
    if isinstance(a, str):
        return xml_matches(parseString(a).documentElement, b)
    if isinstance(b, str):
        return xml_matches(a, parseString(b).documentElement)
    if a.nodeType == xml.dom.Node.DOCUMENT_NODE:
        return xml_matches(a.documentElement, b)
    if b.nodeType == xml.dom.Node.DOCUMENT_NODE:
        return xml_matches(a, b.documentElement)
    if not elements_match(a, b):
        print("Match failed:")
        a.writexml(sys.stdout)
        print("=" * 78)
        b.writexml(sys.stdout)
        return False
    return True


## A python unit test for srdf
class TestSRDFParser(unittest.TestCase):
    ## test valid srdf

    def test_full_srdf(self):
        srdf_data = """
        <robot name="myrobot">
        <group name="body">
          <joint name="J1" />
          <joint name="J2" />
          <joint name="J3" />
          <chain base_link="robot_base" tip_link="robot_tip" />
          <group name="arm" />
        </group>
        <group_state name="zero" group="body">
        <joint name="J1" value="0" />
        <joint name="J2" value="0" />
        <joint name="J3" value="0" />
        </group_state>
        <end_effector name="tip_ee" parent_link="tip" group="arm" parent_group="body" />
        <end_effector name="othertip_ee" parent_link="othertip" group="arm" />
        <virtual_joint name="virtual_joint" type="floating" parent_frame="body_frame" child_link="arm" />
        <disable_collisions link1="link1" link2="link3" />
        <disable_collisions reason="Adjacent"  link1="link1" link2="link2" />
        <link_sphere_approximation link="link1" />
        <link_sphere_approximation link="link2" >
            <sphere center="1.0 2.0 3.0" radius="1.0" />
            <sphere center="1.0 2.0 4.0" radius="2.0" />
        </link_sphere_approximation>
        </robot>
        """
        expected = """
<robot name="myrobot">
  <group name="body">
    <joint name="J1" />
    <joint name="J2" />
    <joint name="J3" />
    <chain base_link="robot_base" tip_link="robot_tip"/>
    <group name="arm" />
  </group>
  <group_state name="zero" group="body">
    <joint name="J1" value="0" />
    <joint name="J2" value="0" />
    <joint name="J3" value="0" />
  </group_state>
  <end_effector group="arm" name="tip_ee" parent_group="body" parent_link="tip"/>
  <end_effector name="othertip_ee" parent_link="othertip" group="arm" />
  <virtual_joint child_link="arm" name="virtual_joint" parent_frame="body_frame" type="floating"  />
  <disable_collisions link1="link1" link2="link3" />
  <disable_collisions link1="link1" link2="link2" reason="Adjacent" />
  <link_sphere_approximation link="link1" />
  <link_sphere_approximation link="link2" >
    <sphere center="1.0 2.0 3.0" radius="1.0" />
    <sphere center="1.0 2.0 4.0" radius="2.0" />
  </link_sphere_approximation>
</robot>
        """
        robot = SRDF.from_xml_string(srdf_data)
        self.assertTrue(xml_matches(robot.to_xml_string(), expected))

    def test_simple_srdf(self):
        datadir = os.path.dirname(os.path.realpath(__file__)) + "/resources/"
        stream = open(datadir + "pr2_desc.1.srdf", "r")
        robot = SRDF.from_xml_string(stream.read())
        stream.close()
        self.assertTrue(len(robot.virtual_joints) == 0)
        self.assertTrue(len(robot.groups) == 0)
        self.assertTrue(len(robot.group_states) == 0)
        self.assertTrue(len(robot.disable_collisionss) == 0)
        self.assertTrue(len(robot.end_effectors) == 0)

        stream = open(datadir + "pr2_desc.2.srdf", "r")
        robot = SRDF.from_xml_string(stream.read())
        stream.close()
        self.assertTrue(len(robot.virtual_joints) == 1)
        self.assertTrue(len(robot.groups) == 1)
        self.assertTrue(len(robot.group_states) == 0)
        self.assertTrue(len(robot.disable_collisionss) == 0)
        self.assertTrue(len(robot.end_effectors) == 0)

    def test_complex_srdf(self):
        datadir = os.path.dirname(os.path.realpath(__file__)) + "/resources/"
        stream = open(datadir + "pr2_desc.3.srdf", "r")
        robot = SRDF.from_xml_string(stream.read())
        stream.close()
        self.assertTrue(len(robot.virtual_joints) == 1)
        self.assertTrue(len(robot.groups) == 7)
        self.assertTrue(len(robot.group_states) == 2)
        self.assertTrue(len(robot.disable_collisionss) == 2)
        self.assertTrue(robot.disable_collisionss[0].reason == "adjacent")
        self.assertTrue(len(robot.end_effectors) == 2)

        self.assertTrue(robot.virtual_joints[0].name == "world_joint")
        self.assertTrue(robot.virtual_joints[0].type == "planar")

        for group in robot.groups:
            if group.name == "left_arm" or group.name == "right_arm":
                self.assertTrue(len(group.chains) == 1)
            if group.name == "arms":
                self.assertTrue(len(group.subgroups) == 2)
            if group.name == "base":
                self.assertTrue(len(group.joints) == 1)
            if group.name == "l_end_effector" or group.name == "r_end_effector":
                self.assertTrue(len(group.links) == 1)
                self.assertTrue(len(group.joints) == 9)
            if group.name == "whole_body":
                self.assertTrue(len(group.joints) == 1)
                self.assertTrue(len(group.subgroups) == 2)

        index = 0
        if robot.group_states[0].group != "arms":
            index = 1

        self.assertTrue(robot.group_states[index].group == "arms")
        self.assertTrue(robot.group_states[index].name == "tuck_arms")
        self.assertTrue(robot.group_states[1 - index].group == "base")
        self.assertTrue(robot.group_states[1 - index].name == "home")

        v = next(
            (
                joint.value
                for joint in robot.group_states[index].joints
                if joint.name == "l_shoulder_pan_joint"
            ),
            None,
        )
        self.assertTrue(len(v) == 1)
        self.assertTrue(v[0] == 0.2)

        w = next(
            (
                joint.value
                for joint in robot.group_states[1 - index].joints
                if joint.name == "world_joint"
            ),
            None,
        )
        self.assertTrue(len(w) == 3)
        self.assertTrue(w[0] == 0.4)
        self.assertTrue(w[1] == 0)
        self.assertTrue(w[2] == -1)

        index = 0 if (robot.end_effectors[0].name[0] == "r") else 1
        self.assertTrue(robot.end_effectors[index].name == "r_end_effector")
        self.assertTrue(robot.end_effectors[index].group == "r_end_effector")
        self.assertTrue(robot.end_effectors[index].parent_link == "r_wrist_roll_link")


if __name__ == "__main__":
    suite = unittest.TestSuite()
    suite.addTest(TestSRDFParser("test_full_srdf"))
    suite.addTest(TestSRDFParser("test_simple_srdf"))
    suite.addTest(TestSRDFParser("test_complex_srdf"))
    unittest.TextTestRunner(verbosity=2).run(suite)

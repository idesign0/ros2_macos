# Copyright 2016-2019 Open Source Robotics Foundation, Inc.
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

import array
import math
import sys

import numpy
import pytest

from rosidl_generator_py.msg import Arrays
from rosidl_generator_py.msg import BasicTypes
from rosidl_generator_py.msg import BoundedSequences
from rosidl_generator_py.msg import BuiltinTypeSequencesIdl
from rosidl_generator_py.msg import Constants
from rosidl_generator_py.msg import Defaults
from rosidl_generator_py.msg import Nested
from rosidl_generator_py.msg import StringArrays
from rosidl_generator_py.msg import Strings
from rosidl_generator_py.msg import UnboundedSequences
from rosidl_generator_py.msg import WStrings

from rosidl_parser.definition import Array
from rosidl_parser.definition import BoundedSequence
from rosidl_parser.definition import BoundedString
from rosidl_parser.definition import NamespacedType
from rosidl_parser.definition import UnboundedSequence
from rosidl_parser.definition import UnboundedString


def test_basic_types():
    msg = BasicTypes()

    # types
    assert isinstance(msg.bool_value, bool)
    assert isinstance(msg.byte_value, bytes)
    assert 1 == len(msg.byte_value)
    # for legacy reasons, 'char' from a .msg interface maps to 'uint8'
    assert isinstance(msg.char_value, int)
    assert isinstance(msg.float32_value, float)
    assert isinstance(msg.float64_value, float)
    assert isinstance(msg.int8_value, int)
    assert isinstance(msg.uint8_value, int)
    assert isinstance(msg.int16_value, int)
    assert isinstance(msg.uint16_value, int)
    assert isinstance(msg.int32_value, int)
    assert isinstance(msg.uint32_value, int)
    assert isinstance(msg.int64_value, int)
    assert isinstance(msg.uint64_value, int)

    # default values
    assert msg.bool_value is False
    assert bytes([0]) == msg.byte_value
    assert 0 == msg.char_value
    assert 0.0 == msg.float32_value
    assert 0.0 == msg.float64_value
    assert 0 == msg.int8_value
    assert 0 == msg.uint8_value
    assert 0 == msg.int16_value
    assert 0 == msg.uint16_value
    assert 0 == msg.int32_value
    assert 0 == msg.uint32_value
    assert 0 == msg.int64_value
    assert 0 == msg.uint64_value

    # assignment
    msg.bool_value = True
    assert msg.bool_value is True
    msg.byte_value = b'2'
    assert bytes([50]) == msg.byte_value
    msg.char_value = 42
    assert 42 == msg.char_value
    msg.float32_value = 1.125
    assert 1.125 == msg.float32_value
    msg.float64_value = 1.125
    assert 1.125 == msg.float64_value
    msg.int8_value = -50
    assert -50 == msg.int8_value
    msg.uint8_value = 200
    assert 200 == msg.uint8_value
    msg.int16_value = -1000
    assert -1000 == msg.int16_value
    msg.uint16_value = 2000
    assert 2000 == msg.uint16_value
    msg.int32_value = -30000
    assert -30000 == msg.int32_value
    msg.uint32_value = 60000
    assert 60000 == msg.uint32_value
    msg.int64_value = -40000000
    assert -40000000 == msg.int64_value
    msg.uint64_value = 50000000
    assert 50000000 == msg.uint64_value

    # out of range
    with pytest.raises(AssertionError):
        setattr(msg, 'char_value', '\x80')
    for i in [8, 16, 32, 64]:
        with pytest.raises(AssertionError):
            setattr(msg, 'int%d_value' % i, 2**(i - 1))
        with pytest.raises(AssertionError):
            setattr(msg, 'int%d_value' % i, -2**(i - 1) - 1)
        with pytest.raises(AssertionError):
            setattr(msg, 'uint%d_value' % i, -1)
        with pytest.raises(AssertionError):
            setattr(msg, 'int%d_value' % i, 2**i)
    float32_ieee_max_next = numpy.nextafter(3.402823466e+38, math.inf)
    with pytest.raises(AssertionError):
        setattr(msg, 'float32_value', -float32_ieee_max_next)
    with pytest.raises(AssertionError):
        setattr(msg, 'float32_value', float32_ieee_max_next)

    # Only run bounds test on system with non-compliant IEEE 754 float64.
    # Otherwise the number is implicitly converted to inf.
    if sys.float_info.max > 1.7976931348623157e+308:
        float64_ieee_max_next = numpy.nextafter(1.7976931348623157e+308, math.inf)
        with pytest.raises(AssertionError):
            setattr(msg, 'float64_value', -float64_ieee_max_next)
        with pytest.raises(AssertionError):
            setattr(msg, 'float64_value', float64_ieee_max_next)

    # NaN
    setattr(msg, 'float32_value', math.nan)
    assert math.isnan(msg.float32_value)
    setattr(msg, 'float64_value', math.nan)
    assert math.isnan(msg.float64_value)

    # -Inf
    setattr(msg, 'float32_value', -math.inf)
    assert math.isinf(msg.float32_value)
    setattr(msg, 'float64_value', -math.inf)
    assert math.isinf(msg.float64_value)

    # +Inf
    setattr(msg, 'float32_value', math.inf)
    assert math.isinf(msg.float32_value)
    setattr(msg, 'float64_value', math.inf)
    assert math.isinf(msg.float64_value)


def test_strings():
    msg = Strings()

    # types
    assert isinstance(msg.string_value, str)
    assert isinstance(msg.string_value_default1, str)
    assert isinstance(msg.bounded_string_value_default1, str)

    # default values
    assert '' == msg.string_value
    assert 'Hello world!' == msg.string_value_default1
    assert "Hello'world!" == msg.string_value_default2
    assert 'Hello"world!' == msg.string_value_default3
    assert "Hello'world!" == msg.string_value_default4
    assert 'Hello"world!' == msg.string_value_default5
    assert 'Hello world!' == Strings.STRING_CONST
    assert '' == msg.bounded_string_value
    assert 'Hello world!' == msg.bounded_string_value_default1
    assert "Hello'world!" == msg.bounded_string_value_default2
    assert 'Hello"world!' == msg.bounded_string_value_default3
    assert "Hello'world!" == msg.bounded_string_value_default4
    assert 'Hello"world!' == msg.bounded_string_value_default5

    # assignment
    msg.string_value = 'foo'
    assert 'foo' == msg.string_value
    msg.string_value_default1 = 'bar'
    assert 'bar' == msg.string_value_default1
    msg.bounded_string_value = 'foo bounded'
    assert 'foo bounded' == msg.bounded_string_value
    msg.bounded_string_value_default1 = 'bar bounded'
    assert 'bar bounded' == msg.bounded_string_value_default1

    # set invalid value type
    with pytest.raises(AssertionError):
        setattr(msg, 'string_value', 1234)

    # get/set invalid attribute
    with pytest.raises(AttributeError):
        setattr(msg, 'invalid_string1', 'foo')
    with pytest.raises(AttributeError):
        getattr(msg, 'invalid_string2')

    # out of bounds
    msg.bounded_string_value = 'a' * 22
    assert 'a' * 22 == msg.bounded_string_value
    with pytest.raises(AssertionError):
        setattr(msg, 'bounded_string_value', 'a' * 23)
    msg.bounded_string_value_default1 = 'a' * 22
    assert 'a' * 22 == msg.bounded_string_value_default1
    with pytest.raises(AssertionError):
        setattr(msg, 'bounded_string_value_default1', 'a' * 23)


def test_wstrings():
    msg = WStrings()

    # types
    assert isinstance(msg.wstring_value, str)
    assert isinstance(msg.wstring_value_default1, str)

    # default values
    assert '' == msg.wstring_value
    assert 'Hello world!' == msg.wstring_value_default1
    assert 'Hellö wörld!' == msg.wstring_value_default2
    assert 'ハローワールド' == msg.wstring_value_default3


def test_arrays_of_bounded_strings():
    msg = StringArrays()
    array_valid_string_length = ['a' * 2, 'b' * 3, 'c' * 4]
    array_too_long_strings = ['a' * 2, 'b' * 3, 'c' * 6]
    assert ['', '', ''] == msg.ub_string_static_array_value
    msg.ub_string_static_array_value = array_valid_string_length
    assert array_valid_string_length == msg.ub_string_static_array_value

    with pytest.raises(AssertionError):
        setattr(msg, 'ub_string_static_array_value', array_too_long_strings)

    with pytest.raises(AssertionError):
        setattr(msg, 'ub_string_static_array_value', ['a' * 2, 'b' * 3])

    assert [] == msg.ub_string_ub_array_value
    msg.ub_string_ub_array_value = array_valid_string_length
    assert array_valid_string_length == msg.ub_string_ub_array_value

    with pytest.raises(AssertionError):
        setattr(msg, 'ub_string_ub_array_value', array_too_long_strings)

    array10strings = [] + [str(i) for i in range(10)]
    msg.ub_string_ub_array_value = array10strings
    assert array10strings == msg.ub_string_ub_array_value

    with pytest.raises(AssertionError):
        setattr(msg, 'ub_string_ub_array_value', array10strings + ['gfg'])

    assert [] == msg.ub_string_dynamic_array_value
    msg.ub_string_dynamic_array_value = array_valid_string_length
    assert array_valid_string_length == msg.ub_string_dynamic_array_value

    with pytest.raises(AssertionError):
        setattr(msg, 'ub_string_dynamic_array_value', array_too_long_strings)

    array10strings = [] + [str(i) for i in range(10)]
    msg.ub_string_dynamic_array_value = array10strings
    assert array10strings == msg.ub_string_dynamic_array_value
    array10strings += ['gfg']
    msg.ub_string_dynamic_array_value = array10strings
    assert array10strings == msg.ub_string_dynamic_array_value


def test_constructor():
    msg = Strings(string_value='foo')

    assert'foo' == msg.string_value

    with pytest.raises(AssertionError):
        Strings(unknown_field='test')


def test_constants():
    assert Constants.BOOL_CONST is True
    assert bytes([50]) == Constants.BYTE_CONST
    assert 100 == Constants.CHAR_CONST
    assert 1.125 == Constants.FLOAT32_CONST
    assert 1.125 == Constants.FLOAT64_CONST
    assert -50 == Constants.INT8_CONST
    assert 200 == Constants.UINT8_CONST
    assert -1000 == Constants.INT16_CONST
    assert 2000 == Constants.UINT16_CONST
    assert -30000 == Constants.INT32_CONST
    assert 60000 == Constants.UINT32_CONST
    assert -40000000 == Constants.INT64_CONST
    assert 50000000 == Constants.UINT64_CONST

    # check we can't overwrite existing constant
    with pytest.raises(AttributeError):
        setattr(Constants, 'INT32_CONST', 42)


def test_default_values():
    msg = Defaults()

    assert msg.bool_value is True
    assert bytes([50]) == msg.byte_value
    assert 100 == msg.char_value
    assert 1.125 == msg.float32_value
    assert 1.125 == msg.float64_value
    assert -50 == msg.int8_value
    assert 200 == msg.uint8_value
    assert -1000 == msg.int16_value
    assert 2000 == msg.uint16_value
    assert -30000 == msg.int32_value
    assert 60000 == msg.uint32_value
    assert -40000000 == msg.int64_value
    assert 50000000 == msg.uint64_value

    # overwrite value
    msg.int32_value = 42
    assert 42 == msg.int32_value
    # check actual default is not changed
    assert -30000 == Defaults.INT32_VALUE__DEFAULT
    # check that we can't change default
    with pytest.raises(AttributeError):
        setattr(Defaults, 'INT32_VALUE__DEFAULT', 24)


def test_arrays():
    msg = Arrays()

    # types
    assert isinstance(msg.bool_values, list)
    assert isinstance(msg.byte_values, list)
    # for legacy reasons, 'char' from a .msg interface maps to 'uint8'
    assert isinstance(msg.char_values, numpy.ndarray)
    assert isinstance(msg.float32_values, numpy.ndarray)
    assert isinstance(msg.float64_values, numpy.ndarray)
    assert isinstance(msg.int8_values, numpy.ndarray)
    assert isinstance(msg.uint8_values, numpy.ndarray)
    assert isinstance(msg.int16_values, numpy.ndarray)
    assert isinstance(msg.uint16_values, numpy.ndarray)
    assert isinstance(msg.int32_values, numpy.ndarray)
    assert isinstance(msg.uint32_values, numpy.ndarray)
    assert isinstance(msg.int64_values, numpy.ndarray)
    assert isinstance(msg.uint64_values, numpy.ndarray)

    # TODO(jacobperron): Add checks for remaining fields

    # defaults
    assert 3 == len(msg.bool_values)
    assert 3 == len(msg.byte_values)
    assert 3 == len(msg.char_values)
    assert 3 == len(msg.float32_values)
    assert 3 == len(msg.float64_values)
    assert 3 == len(msg.int8_values)
    assert 3 == len(msg.uint8_values)
    assert 3 == len(msg.int16_values)
    assert 3 == len(msg.uint16_values)
    assert 3 == len(msg.int32_values)
    assert 3 == len(msg.uint32_values)
    assert 3 == len(msg.int64_values)
    assert 3 == len(msg.uint64_values)

    # valid assignment
    list_of_bool = [True, False, True]
    msg.bool_values = list_of_bool
    assert list_of_bool == msg.bool_values
    list_of_byte = [b'a', b'b', b'c']
    msg.byte_values = list_of_byte
    assert list_of_byte == msg.byte_values
    list_of_char = [0, 1, 255]
    msg.char_values = list_of_char
    assert numpy.array_equal(list_of_char, msg.char_values)
    list_of_float32 = [0.0, -1.125, 1.125]
    msg.float32_values = list_of_float32
    assert numpy.allclose(list_of_float32, msg.float32_values)
    list_of_float64 = [0.0, 1.125, 1.125]
    msg.float64_values = list_of_float64
    assert numpy.allclose(list_of_float64, msg.float64_values)
    list_of_int8 = [0, -128, 127]
    msg.int8_values = list_of_int8
    assert numpy.array_equal(list_of_int8, msg.int8_values)
    list_of_uint8 = [0, 1, 255]
    msg.uint8_values = list_of_uint8
    assert numpy.array_equal(list_of_uint8, msg.uint8_values)
    list_of_int16 = [0, -32768, 32767]
    msg.int16_values = list_of_int16
    assert numpy.array_equal(list_of_int16, msg.int16_values)
    list_of_uint16 = [0, 1, 65535]
    msg.uint16_values = list_of_uint16
    assert numpy.array_equal(list_of_uint16, msg.uint16_values)
    list_of_int32 = [0, -2147483648, 2147483647]
    msg.int32_values = list_of_int32
    assert numpy.array_equal(list_of_int32, msg.int32_values)
    list_of_uint32 = [0, 1, 4294967295]
    msg.uint32_values = list_of_uint32
    assert numpy.array_equal(list_of_uint32, msg.uint32_values)
    list_of_int64 = [0, -9223372036854775808, 9223372036854775807]
    msg.int64_values = list_of_int64
    assert numpy.array_equal(list_of_int64, msg.int64_values)
    list_of_uint64 = [0, 1, 18446744073709551615]
    msg.uint64_values = list_of_uint64
    assert numpy.array_equal(list_of_uint64, msg.uint64_values)

    # assign wrong size
    with pytest.raises(AssertionError):
        msg.bool_values = [True]
    with pytest.raises(AssertionError):
        msg.byte_values = [b'd']
    with pytest.raises(AssertionError):
        msg.char_values = [0]
    with pytest.raises(AssertionError):
        msg.float32_values = [1.125]
    with pytest.raises(AssertionError):
        msg.float64_values = [1.125]
    with pytest.raises(AssertionError):
        msg.int8_values = [42]
    with pytest.raises(AssertionError):
        msg.uint8_values = [42]
    with pytest.raises(AssertionError):
        msg.int16_values = [42]
    with pytest.raises(AssertionError):
        msg.uint16_values = [42]
    with pytest.raises(AssertionError):
        msg.int32_values = [42]
    with pytest.raises(AssertionError):
        msg.uint32_values = [42]
    with pytest.raises(AssertionError):
        msg.int64_values = [42]
    with pytest.raises(AssertionError):
        msg.uint64_values = [42]

    # invalid type
    with pytest.raises(AssertionError):
        msg.bool_values = ['not', 'a', 'bool']
    with pytest.raises(AssertionError):
        msg.byte_values = ['not', 'a', 'byte']
    with pytest.raises(AssertionError):
        msg.char_values = ['not', 'a', 'char']
    with pytest.raises(AssertionError):
        msg.float32_values = ['not', 'a', 'float32']
    with pytest.raises(AssertionError):
        msg.float64_values = ['not', 'a', 'float64']
    with pytest.raises(AssertionError):
        msg.int8_values = ['not', 'a', 'int8']
    with pytest.raises(AssertionError):
        msg.uint8_values = ['not', 'a', 'uint8']
    with pytest.raises(AssertionError):
        msg.int16_values = ['not', 'a', 'int16']
    with pytest.raises(AssertionError):
        msg.uint16_values = ['not', 'a', 'uint16']
    with pytest.raises(AssertionError):
        msg.int32_values = ['not', 'a', 'int32']
    with pytest.raises(AssertionError):
        msg.uint32_values = ['not', 'a', 'uint32']
    with pytest.raises(AssertionError):
        msg.int64_values = ['not', 'a', 'int64']
    with pytest.raises(AssertionError):
        msg.uint64_values = ['not', 'a', 'uint64']

    # out of range
    with pytest.raises(AssertionError):
        setattr(msg, 'char_values', [2**8, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'char_values', [-1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int8_values', [2**8, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int8_values', [-2**7 - 1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint8_values', [2**8, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint8_values', [-1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int16_values', [2**16, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int16_values', [-2**15 - 1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint16_values', [2**16, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint16_values', [-1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int32_values', [2**32, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int32_values', [-2**31 - 1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint32_values', [2**32, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint32_values', [-1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int64_values', [2**64, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int64_values', [-2**63 - 1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint64_values', [2**64, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint64_values', [-1, 1, 2])
    with pytest.raises(AssertionError):
        float32_ieee_max_next = numpy.nextafter(3.402823466e+38, math.inf)
        setattr(msg, 'float32_values', [-float32_ieee_max_next, 0.0])
    with pytest.raises(AssertionError):
        float32_ieee_max_next = numpy.nextafter(3.402823466e+38, math.inf)
        setattr(msg, 'float32_values', [0.0, float32_ieee_max_next])

    # If target system is IEEE 754 compliant, the next number is rounded to inf.
    # Only perform this check on non-compliant systems.
    if sys.float_info.max > 1.7976931348623157e+308:
        with pytest.raises(AssertionError):
            float64_ieee_max_next = numpy.nextafter(1.7976931348623157e+308, math.inf)
            setattr(msg, 'float64_values', [-float64_ieee_max_next, 0.0])
        with pytest.raises(AssertionError):
            float64_ieee_max_next = numpy.nextafter(1.7976931348623157e+308, math.inf)
            setattr(msg, 'float64_values', [-float64_ieee_max_next, 0.0])

    # NaN
    arr_of_float32_with_nan = numpy.array([-1.33, math.nan, 1.33], dtype=numpy.float32)
    setattr(msg, 'float32_values', arr_of_float32_with_nan)
    arr_with_nan = numpy.isnan(arr_of_float32_with_nan)
    assert numpy.shape(arr_of_float32_with_nan) == numpy.shape(msg.float32_values) and (
        numpy.asarray(arr_of_float32_with_nan[~arr_with_nan] ==
                      msg.float32_values[~arr_with_nan]).all())
    arr_of_float64_with_nan = numpy.array([-1.66, math.nan, 1.66], dtype=numpy.float64)
    setattr(msg, 'float64_values', arr_of_float64_with_nan)
    arr_with_nan = numpy.isnan(arr_of_float64_with_nan)
    assert numpy.shape(arr_of_float64_with_nan) == numpy.shape(msg.float64_values) and (
        numpy.asarray(arr_of_float64_with_nan[~arr_with_nan] ==
                      msg.float64_values[~arr_with_nan]).all())
    # Inf
    arr_of_float32_with_inf = numpy.array([-math.inf, 5.5, math.inf], dtype=numpy.float32)
    setattr(msg, 'float32_values', arr_of_float32_with_inf)
    assert numpy.array_equal(arr_of_float32_with_inf, msg.float32_values)
    arr_of_float64_with_inf = numpy.array([-math.inf, 5.5, math.inf], dtype=numpy.float64)
    setattr(msg, 'float64_values', arr_of_float64_with_inf)
    assert numpy.array_equal(arr_of_float64_with_inf, msg.float64_values)

    msg2 = Arrays(int8_values=[1, 2, 3])
    msg3 = Arrays(int8_values=[4, 5, 3])

    assert msg2 != msg3


def test_bounded_sequences():
    msg = BoundedSequences()

    # types
    assert isinstance(msg.bool_values, list)
    assert isinstance(msg.byte_values, list)
    # for legacy reasons, 'char' from a .msg interface maps to 'uint8'
    assert isinstance(msg.char_values, array.array)
    assert isinstance(msg.float32_values, array.array)
    assert isinstance(msg.float64_values, array.array)
    assert isinstance(msg.int8_values, array.array)
    assert isinstance(msg.uint8_values, array.array)
    assert isinstance(msg.int16_values, array.array)
    assert isinstance(msg.uint16_values, array.array)
    assert isinstance(msg.int32_values, array.array)
    assert isinstance(msg.uint32_values, array.array)
    assert isinstance(msg.int64_values, array.array)
    assert isinstance(msg.uint64_values, array.array)

    # TODO(jacobperron): Add checks for remaining fields

    # defaults
    assert [] == msg.bool_values
    assert [] == msg.byte_values
    assert array.array('B') == msg.char_values
    assert array.array('f') == msg.float32_values
    assert array.array('d') == msg.float64_values
    assert array.array('b') == msg.int8_values
    assert array.array('B') == msg.uint8_values
    assert array.array('h') == msg.int16_values
    assert array.array('H') == msg.uint16_values
    assert array.array('l') == msg.int32_values
    assert array.array('L') == msg.uint32_values
    assert array.array('q') == msg.int64_values
    assert array.array('Q') == msg.uint64_values

    # valid assignment
    list_of_bool = [True, False, True]
    short_list_of_bool = [False]
    msg.bool_values = list_of_bool
    assert list_of_bool == msg.bool_values
    msg.bool_values = short_list_of_bool
    assert short_list_of_bool == msg.bool_values
    list_of_byte = [b'a', b'b', b'c']
    short_list_of_byte = [b'd']
    msg.byte_values = list_of_byte
    assert list_of_byte == msg.byte_values
    msg.byte_values = short_list_of_byte
    assert short_list_of_byte == msg.byte_values
    list_of_char = [0, 1, 255]
    short_list_of_char = [0]
    msg.char_values = list_of_char
    assert array.array('B', list_of_char) == msg.char_values
    msg.char_values = short_list_of_char
    assert array.array('B', short_list_of_char) == msg.char_values
    list_of_float32 = [0.1, 2.3, -3.14]
    short_list_of_float32 = [1.125]
    msg.float32_values = list_of_float32
    assert array.array('f', list_of_float32) == msg.float32_values
    msg.float32_values = short_list_of_float32
    assert array.array('d', short_list_of_float32) == msg.float32_values
    list_of_float64 = [0.1, 2.3, -3.14]
    short_list_of_float64 = [1.125]
    msg.float64_values = list_of_float64
    assert array.array('d', list_of_float64) == msg.float64_values
    msg.float64_values = short_list_of_float64
    assert array.array('d', short_list_of_float64) == msg.float64_values
    list_of_int8 = [0, -128, 127]
    short_list_of_int8 = [1]
    msg.int8_values = list_of_int8
    assert array.array('b', list_of_int8) == msg.int8_values
    msg.int8_values = short_list_of_int8
    assert array.array('b', short_list_of_int8) == msg.int8_values
    list_of_uint8 = [0, 1, 255]
    short_list_of_uint8 = [1]
    msg.uint8_values = list_of_uint8
    assert array.array('B', list_of_uint8) == msg.uint8_values
    msg.uint8_values = short_list_of_uint8
    assert array.array('B', short_list_of_uint8) == msg.uint8_values
    list_of_int16 = [0, -32768, 32767]
    short_list_of_int16 = [1]
    msg.int16_values = list_of_int16
    assert array.array('h', list_of_int16) == msg.int16_values
    msg.int16_values = short_list_of_int16
    assert array.array('h', short_list_of_int16) == msg.int16_values
    list_of_uint16 = [0, 1, 65535]
    short_list_of_uint16 = [1]
    msg.uint16_values = list_of_uint16
    assert array.array('H', list_of_uint16) == msg.uint16_values
    msg.uint16_values = short_list_of_uint16
    assert array.array('H', short_list_of_uint16) == msg.uint16_values
    list_of_int32 = [0, -2147483648, 2147483647]
    short_list_of_int32 = [1]
    msg.int32_values = list_of_int32
    assert array.array('l', list_of_int32) == msg.int32_values
    msg.int32_values = short_list_of_int32
    assert array.array('l', short_list_of_int32) == msg.int32_values
    list_of_uint32 = [0, 1, 4294967295]
    short_list_of_uint32 = [1]
    msg.uint32_values = list_of_uint32
    assert array.array('L', list_of_uint32) == msg.uint32_values
    msg.uint32_values = short_list_of_uint32
    assert array.array('L', short_list_of_uint32) == msg.uint32_values
    list_of_int64 = [0, -9223372036854775808, 9223372036854775807]
    short_list_of_int64 = [1]
    msg.int64_values = list_of_int64
    assert array.array('q', list_of_int64) == msg.int64_values
    msg.int64_values = short_list_of_int64
    assert array.array('q', short_list_of_int64) == msg.int64_values
    list_of_uint64 = [0, 1, 18446744073709551615]
    short_list_of_uint64 = [1]
    msg.uint64_values = list_of_uint64
    assert array.array('Q', list_of_uint64) == msg.uint64_values
    msg.uint64_values = short_list_of_uint64
    assert array.array('Q', short_list_of_uint64) == msg.uint64_values

    # too many in sequence
    with pytest.raises(AssertionError):
        msg.bool_values = [True, False, True, False]
    with pytest.raises(AssertionError):
        msg.byte_values = [b'a', b'b', b'c', b'd']
    with pytest.raises(AssertionError):
        msg.char_values = [1, 2, 3, 4]
    with pytest.raises(AssertionError):
        msg.float32_values = [1.0, 2.0, 3.0, 4.0]
    with pytest.raises(AssertionError):
        msg.float64_values = [1.0, 2.0, 3.0, 4.0]
    with pytest.raises(AssertionError):
        msg.int8_values = [1, 2, 3, 4]
    with pytest.raises(AssertionError):
        msg.uint8_values = [1, 2, 3, 4]
    with pytest.raises(AssertionError):
        msg.int16_values = [1, 2, 3, 4]
    with pytest.raises(AssertionError):
        msg.uint16_values = [1, 2, 3, 4]
    with pytest.raises(AssertionError):
        msg.int32_values = [1, 2, 3, 4]
    with pytest.raises(AssertionError):
        msg.uint32_values = [1, 2, 3, 4]
    with pytest.raises(AssertionError):
        msg.int64_values = [1, 2, 3, 4]
    with pytest.raises(AssertionError):
        msg.uint64_values = [1, 2, 3, 4]

    # invalid type
    with pytest.raises(AssertionError):
        msg.bool_values = ['not', 'a', 'bool']
    with pytest.raises(AssertionError):
        msg.byte_values = ['not', 'a', 'byte']
    with pytest.raises(AssertionError):
        msg.char_values = ['not', 'a', 'char']
    with pytest.raises(AssertionError):
        msg.float32_values = ['not', 'a', 'float32']
    with pytest.raises(AssertionError):
        msg.float64_values = ['not', 'a', 'float64']
    with pytest.raises(AssertionError):
        msg.int8_values = ['not', 'a', 'int8']
    with pytest.raises(AssertionError):
        msg.uint8_values = ['not', 'a', 'uint8']
    with pytest.raises(AssertionError):
        msg.int16_values = ['not', 'a', 'int16']
    with pytest.raises(AssertionError):
        msg.uint16_values = ['not', 'a', 'uint16']
    with pytest.raises(AssertionError):
        msg.int32_values = ['not', 'a', 'int32']
    with pytest.raises(AssertionError):
        msg.uint32_values = ['not', 'a', 'uint32']
    with pytest.raises(AssertionError):
        msg.int64_values = ['not', 'a', 'int64']
    with pytest.raises(AssertionError):
        msg.uint64_values = ['not', 'a', 'uint64']

    # out of range
    with pytest.raises(AssertionError):
        setattr(msg, 'char_values', [2**8, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'char_values', [-1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int8_values', [2**8, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int8_values', [-2**7 - 1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint8_values', [2**8, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint8_values', [-1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int16_values', [2**16, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int16_values', [-2**15 - 1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint16_values', [2**16, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint16_values', [-1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int32_values', [2**32, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int32_values', [-2**31 - 1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint32_values', [2**32, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint32_values', [-1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int64_values', [2**64, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int64_values', [-2**63 - 1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint64_values', [2**64, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint64_values', [-1, 1, 2])
    with pytest.raises(AssertionError):
        float32_ieee_max_next = numpy.nextafter(3.402823466e+38, math.inf)
        setattr(msg, 'float32_values', [-float32_ieee_max_next, 0.0])
    with pytest.raises(AssertionError):
        float32_ieee_max_next = numpy.nextafter(3.402823466e+38, math.inf)
        setattr(msg, 'float32_values', [0.0, float32_ieee_max_next])

    # If target system is IEEE 754 compliant, the next number is rounded to inf.
    # Only perform this check on non-compliant systems.
    if sys.float_info.max > 1.7976931348623157e+308:
        with pytest.raises(AssertionError):
            float64_ieee_max_next = numpy.nextafter(1.7976931348623157e+308, math.inf)
            setattr(msg, 'float64_values', [-float64_ieee_max_next, 0.0, float64_ieee_max_next])


def test_unbounded_sequences():
    msg = UnboundedSequences()

    # types
    assert isinstance(msg.byte_values, list)
    # for legacy reasons, 'char' from a .msg interface maps to 'uint8'
    assert isinstance(msg.char_values, array.array)
    assert isinstance(msg.float32_values, array.array)
    assert isinstance(msg.float64_values, array.array)
    assert isinstance(msg.int8_values, array.array)
    assert isinstance(msg.uint8_values, array.array)
    assert isinstance(msg.int16_values, array.array)
    assert isinstance(msg.uint16_values, array.array)
    assert isinstance(msg.int32_values, array.array)
    assert isinstance(msg.uint32_values, array.array)
    assert isinstance(msg.int64_values, array.array)
    assert isinstance(msg.uint64_values, array.array)

    # TODO(jacobperron): Add checks for remaining fields

    # defaults
    assert [] == msg.bool_values
    assert [] == msg.byte_values
    assert array.array('B') == msg.char_values
    assert array.array('f') == msg.float32_values
    assert array.array('d') == msg.float64_values
    assert array.array('b') == msg.int8_values
    assert array.array('B') == msg.uint8_values
    assert array.array('h') == msg.int16_values
    assert array.array('H') == msg.uint16_values
    assert array.array('l') == msg.int32_values
    assert array.array('L') == msg.uint32_values
    assert array.array('q') == msg.int64_values
    assert array.array('Q') == msg.uint64_values

    # assignment
    list_of_bool = [True, False, True]
    msg.bool_values = list_of_bool
    assert list_of_bool == msg.bool_values
    list_of_byte = [b'a', b'b', b'c']
    msg.byte_values = list_of_byte
    assert list_of_byte == msg.byte_values
    list_of_char = [0, 1, 255]
    msg.char_values = list_of_char
    assert array.array('B', list_of_char) == msg.char_values
    list_of_float32 = [0.1, 2.3, -3.14]
    msg.float32_values = list_of_float32
    assert array.array('f', list_of_float32) == msg.float32_values
    list_of_float64 = [0.1, 2.3, -3.14]
    msg.float64_values = list_of_float64
    assert array.array('d', list_of_float64) == msg.float64_values
    list_of_int8 = [0, -128, 127]
    msg.int8_values = list_of_int8
    assert array.array('b', list_of_int8) == msg.int8_values
    list_of_uint8 = [0, 1, 255]
    msg.uint8_values = list_of_uint8
    assert array.array('B', list_of_uint8) == msg.uint8_values
    list_of_int16 = [0, -32768, 32767]
    msg.int16_values = list_of_int16
    assert array.array('h', list_of_int16) == msg.int16_values
    list_of_uint16 = [0, 1, 65535]
    msg.uint16_values = list_of_uint16
    assert array.array('H', list_of_uint16) == msg.uint16_values
    list_of_int32 = [0, -2147483648, 2147483647]
    msg.int32_values = list_of_int32
    assert array.array('l', list_of_int32) == msg.int32_values
    list_of_uint32 = [0, 1, 4294967295]
    msg.uint32_values = list_of_uint32
    assert array.array('L', list_of_uint32) == msg.uint32_values
    list_of_int64 = [0, -9223372036854775808, 9223372036854775807]
    msg.int64_values = list_of_int64
    assert array.array('q', list_of_int64) == msg.int64_values
    list_of_uint64 = [0, 1, 18446744073709551615]
    msg.uint64_values = list_of_uint64
    assert array.array('Q', list_of_uint64) == msg.uint64_values

    # invalid type
    with pytest.raises(AssertionError):
        msg.bool_values = ['not', 'a', 'bool']
    with pytest.raises(AssertionError):
        msg.byte_values = ['not', 'a', 'byte']
    with pytest.raises(AssertionError):
        msg.char_values = ['not', 'a', 'char']
    with pytest.raises(AssertionError):
        msg.float32_values = ['not', 'a', 'float32']
    with pytest.raises(AssertionError):
        msg.float64_values = ['not', 'a', 'float64']
    with pytest.raises(AssertionError):
        msg.int8_values = ['not', 'a', 'int8']
    with pytest.raises(AssertionError):
        msg.uint8_values = ['not', 'a', 'uint8']
    with pytest.raises(AssertionError):
        msg.int16_values = ['not', 'a', 'int16']
    with pytest.raises(AssertionError):
        msg.uint16_values = ['not', 'a', 'uint16']
    with pytest.raises(AssertionError):
        msg.int32_values = ['not', 'a', 'int32']
    with pytest.raises(AssertionError):
        msg.uint32_values = ['not', 'a', 'uint32']
    with pytest.raises(AssertionError):
        msg.int64_values = ['not', 'a', 'int64']
    with pytest.raises(AssertionError):
        msg.uint64_values = ['not', 'a', 'uint64']

    # out of range
    with pytest.raises(AssertionError):
        setattr(msg, 'int8_values', [2**8, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int8_values', [-2**7 - 1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint8_values', [2**8, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint8_values', [-1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int16_values', [2**16, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int16_values', [-2**15 - 1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint16_values', [2**16, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint16_values', [-1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int32_values', [2**32, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int32_values', [-2**31 - 1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint32_values', [2**32, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint32_values', [-1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int64_values', [2**64, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'int64_values', [-2**63 - 1, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint64_values', [2**64, 1, 2])
    with pytest.raises(AssertionError):
        setattr(msg, 'uint64_values', [-1, 1, 2])
    with pytest.raises(AssertionError):
        float32_ieee_max_next = numpy.nextafter(3.402823466e+38, math.inf)
        setattr(msg, 'float32_values', [-float32_ieee_max_next, 0.0, float32_ieee_max_next])

    # If target system is IEEE 754 compliant, the next number is rounded to inf.
    # Only perform this check on non-compliant systems.
    if sys.float_info.max > 1.7976931348623157e+308:
        with pytest.raises(AssertionError):
            float64_ieee_max_next = numpy.nextafter(1.7976931348623157e+308, math.inf)
            setattr(msg, 'float64_values', [-float64_ieee_max_next, 0.0, float64_ieee_max_next])


def test_slot_attributes():
    msg = Nested()
    assert hasattr(msg, 'get_fields_and_field_types')
    assert hasattr(msg, '__slots__')
    nested_slot_types_dict = getattr(msg, 'get_fields_and_field_types')()
    nested_slots = getattr(msg, '__slots__')
    assert len(nested_slot_types_dict) == len(nested_slots)
    expected_nested_slot_types_dict = {
        'basic_types_value': 'rosidl_generator_py/BasicTypes',
    }
    assert len(nested_slot_types_dict) == len(expected_nested_slot_types_dict)

    for expected_field, expected_slot_type in expected_nested_slot_types_dict.items():
        assert expected_field in nested_slot_types_dict.keys()
        assert expected_slot_type == nested_slot_types_dict[expected_field]


def test_string_slot_attributes():
    msg = StringArrays()
    assert hasattr(msg, 'get_fields_and_field_types')
    assert hasattr(msg, '__slots__')
    string_slot_types_dict = getattr(msg, 'get_fields_and_field_types')()
    string_slots = getattr(msg, '__slots__')
    assert len(string_slot_types_dict) == len(string_slots)
    expected_string_slot_types_dict = {
        'ub_string_static_array_value': 'string<5>[3]',
        'ub_string_ub_array_value': 'sequence<string<5>, 10>',
        'ub_string_dynamic_array_value': 'sequence<string<5>>',
        'string_dynamic_array_value': 'sequence<string>',
        'string_static_array_value': 'string[3]',
        'string_bounded_array_value': 'sequence<string, 10>',
        'def_string_dynamic_array_value': 'sequence<string>',
        'def_string_static_array_value': 'string[3]',
        'def_string_bounded_array_value': 'sequence<string, 10>',
        'def_various_quotes': 'sequence<string>',
        'def_various_commas': 'sequence<string>',
    }

    assert len(string_slot_types_dict) == len(expected_string_slot_types_dict)

    for expected_field, expected_slot_type in expected_string_slot_types_dict.items():
        assert expected_field in string_slot_types_dict.keys()
        assert expected_slot_type == string_slot_types_dict[expected_field]


def test_modifying_slot_fields_and_types():
    msg = StringArrays()
    assert hasattr(msg, 'get_fields_and_field_types')
    string_slot_types_dict = getattr(msg, 'get_fields_and_field_types')()
    string_slot_types_dict_len = len(string_slot_types_dict)
    string_slot_types_dict[1] = 2
    assert len(getattr(msg, 'get_fields_and_field_types')()) == string_slot_types_dict_len


def test_slot_types():
    msg = Nested()
    assert hasattr(msg, 'SLOT_TYPES')
    assert hasattr(msg, '__slots__')
    nested_slot_types = Nested.SLOT_TYPES
    nested_slots = getattr(msg, '__slots__')
    assert len(nested_slot_types) == len(nested_slots)
    assert isinstance(nested_slot_types[0], NamespacedType)
    assert nested_slot_types[0].namespaces == ['rosidl_generator_py', 'msg']
    assert nested_slot_types[0].name == 'BasicTypes'


def test_string_slot_types():
    msg = StringArrays()
    assert hasattr(msg, 'SLOT_TYPES')
    assert hasattr(msg, '__slots__')
    string_slot_types = StringArrays.SLOT_TYPES
    string_slots = getattr(msg, '__slots__')
    assert len(string_slot_types) == len(string_slots)

    assert isinstance(string_slot_types[0], Array)
    assert isinstance(string_slot_types[0].value_type, BoundedString)
    assert string_slot_types[0].size == 3
    assert string_slot_types[0].value_type.maximum_size == 5

    assert isinstance(string_slot_types[1], BoundedSequence)
    assert isinstance(string_slot_types[1].value_type, BoundedString)
    assert string_slot_types[1].maximum_size == 10
    assert string_slot_types[1].value_type.maximum_size == 5

    assert isinstance(string_slot_types[2], UnboundedSequence)
    assert isinstance(string_slot_types[2].value_type, BoundedString)
    assert string_slot_types[2].value_type.maximum_size == 5

    assert isinstance(string_slot_types[3], UnboundedSequence)
    assert isinstance(string_slot_types[3].value_type, UnboundedString)

    assert isinstance(string_slot_types[4], Array)
    assert isinstance(string_slot_types[4].value_type, UnboundedString)
    assert string_slot_types[4].size == 3


def test_builtin_sequence_slot_attributes():
    msg = BuiltinTypeSequencesIdl()
    assert hasattr(msg, 'get_fields_and_field_types')
    assert hasattr(msg, '__slots__')
    builtin_sequence_slot_types_dict = getattr(msg, 'get_fields_and_field_types')()
    builtin_sequence_slots = getattr(msg, '__slots__')
    assert len(builtin_sequence_slot_types_dict) == len(builtin_sequence_slots)

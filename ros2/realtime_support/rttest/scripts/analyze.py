#!/usr/bin/env python3
# Copyright 2015 Open Source Robotics Foundation, Inc.
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

import argparse

import numpy

parser = argparse.ArgumentParser(description='Plot rttest output')
parser.add_argument('filename', metavar='filename')
args = parser.parse_args()
filename = args.filename

outfile = filename + '_plot'

rawlines = []

with open(filename) as f:
    rawlines = f.readlines()
rawlines = [line.rstrip().split(' ') for line in rawlines]
array = numpy.array(rawlines)
latency = numpy.absolute(array[1:, 2].astype(int))

min_latency = numpy.min(latency)
max_latency = numpy.max(latency)
mean_latency = numpy.mean(latency)

print('Min latency:', min_latency)
print('Max latency:', max_latency)
print('Mean latency:', mean_latency)

# How many samples were above 0.03 ms (30000 ns)?

indices = numpy.where(latency > 30000)
print('# of samples overrun:', len(indices))

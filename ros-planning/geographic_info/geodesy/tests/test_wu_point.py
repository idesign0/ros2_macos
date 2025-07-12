#!/usr/bin/env python

import unittest
import uuid

from geographic_msgs.msg import GeographicMap
from geographic_msgs.msg import GeoPoint
from geographic_msgs.msg import WayPoint
from geometry_msgs.msg import Point
from unique_identifier_msgs.msg import UUID

from geodesy.wu_point import *

suite = unittest.TestSuite()


def fromLatLong(lat, lon, alt=float('nan')):
    """Generate WayPoint from latitude, longitude and (optional) altitude.

    :returns: minimal WayPoint object just for test cases.
    """
    geo_pt = GeoPoint(latitude = lat, longitude = lon, altitude = alt)
    return WayPoint(position = geo_pt)

class TestWuPoint(unittest.TestCase):
    """Unit tests for WuPoint classes.
    """

    def test_real_point(self):
        ll = GeoPoint(latitude = 30.385315,
                      longitude = -97.728524,
                      altitude = 209.0)
        msg = WayPoint(position = ll)
        pt = WuPoint(msg)
        self.assertEqual(pt.toWayPoint(), msg)
        self.assertEqual(str(pt.utm),
                         'UTM: [622159.338, 3362168.303, 209.000, 14R]')

        point_xyz = pt.toPoint()
        self.assertAlmostEqual(point_xyz.x, 622159.338, places = 3)
        self.assertAlmostEqual(point_xyz.y, 3362168.303, places = 3)
        self.assertAlmostEqual(point_xyz.z, 209.0, places = 3)

        point_xy = pt.toPointXY()
        self.assertAlmostEqual(point_xy.x, 622159.338, places = 3)
        self.assertAlmostEqual(point_xy.y, 3362168.303, places = 3)
        self.assertAlmostEqual(point_xy.z, 0.0, places = 3)

    def test_valid_points(self):
        lon = -177.0
        zone = 1
        while lon < 180.0:
            pt = WuPoint(fromLatLong(-80.0, lon))
            self.assertEqual(pt.utm.gridZone(), (zone, 'C'))
            pt = WuPoint(fromLatLong(-30.385315, lon))
            self.assertEqual(pt.utm.gridZone(), (zone, 'J'))
            pt = WuPoint(fromLatLong(-0.000001, lon))
            self.assertEqual(pt.utm.gridZone(), (zone, 'M'))
            pt = WuPoint(fromLatLong(0.0, lon))
            self.assertEqual(pt.utm.gridZone(), (zone, 'N'))
            pt = WuPoint(fromLatLong(30.385315, lon))
            self.assertEqual(pt.utm.gridZone(), (zone, 'R'))
            pt = WuPoint(fromLatLong(84.0, lon))
            self.assertEqual(pt.utm.gridZone(), (zone, 'X'))
            lon += 6.0
            zone += 1

    def test_invalid_points(self):
        self.assertRaises(ValueError, WuPoint,
                          fromLatLong(90.385315, -97.728524))
        self.assertRaises(ValueError, WuPoint,
                          fromLatLong(30.385315, -197.728524))
        # this will be valid when we add UPS support for the poles:
        self.assertRaises(ValueError, WuPoint,
                          fromLatLong(-80.385315,-97.728524))

    def test_empty_point_set(self):
        # test WuPointSet iterator with empty list
        wupts = WuPointSet(GeographicMap().points)
        i = 0
        for w in wupts:
            self.fail(msg='there are no points in this map')
            i += 1
        self.assertEqual(i, 0)

        uu = 'da7c242f-2efe-5175-9961-49cc621b80b9'
        self.assertEqual(wupts.get(uu), None)

    def test_three_point_set(self):
        # test data
        uuids = ['da7c242f-2efe-5175-9961-49cc621b80b9',
                 '812f1c08-a34b-5a21-92b9-18b2b0cf4950',
                 '6f0606f6-a776-5940-b5ea-5e889b32c712']
        latitudes = [30.3840168, 30.3857290, 30.3866750]
        longitudes = [-97.7282100, -97.7316754, -97.7270967]
        eastings = [622191.124, 621856.023, 622294.785]
        northings = [3362024.764, 3362210.790, 3362320.569]

        points = []
        for i in range(len(uuids)):
            latlon = GeoPoint(latitude = latitudes[i],
                              longitude = longitudes[i])
            points.append(WayPoint(id = UUID(uuid = list(uuid.UUID(uuids[i]).bytes)),
                                   position = latlon))
    
        # test iterator
        wupts = WuPointSet(points)
        i = 0
        for w in wupts:
            uuid_msg = UUID(uuid=list(uuid.UUID(uuids[i]).bytes))

            self.assertEqual(str(uuid.UUID(bytes=bytes(w.uuid()), version=5)), uuids[i])
            self.assertEqual(str(uuid.UUID(bytes=bytes(wupts[str(uuid_msg.uuid)].uuid()), version=5)), uuids[i])

            self.assertAlmostEqual(w.utm.easting, eastings[i], places=3)
            self.assertAlmostEqual(w.utm.northing, northings[i], places=3)
            point_xy = w.toPointXY()
            self.assertAlmostEqual(point_xy.x, eastings[i], places = 3)
            self.assertAlmostEqual(point_xy.y, northings[i], places = 3)
            self.assertAlmostEqual(point_xy.z, 0.0, places = 3)
            i += 1
        self.assertEqual(i, 3)
        self.assertEqual(len(wupts), 3)

        bogus = '00000000-c433-5c42-be2e-fbd97ddff9ac'
        self.assertFalse(bogus in wupts)
        self.assertEqual(wupts.get(bogus), None)

        uu = str(UUID(uuid=list(uuid.UUID(uuids[1]).bytes)).uuid)
        self.assertTrue(uu in wupts)
        wpt = wupts[uu]
        self.assertEqual(str(wpt.uuid()), uu)

        self.assertNotEqual(wupts.get(uu), None)
        self.assertEqual(str(wupts.get(uu).uuid()), uu)

        # test index() function
        for i in range(len(uuids)):
            wpuuid = str(UUID(uuid=list(uuid.UUID(uuids[i]).bytes)).uuid)
            self.assertEqual(wupts.index(wpuuid), i)
            self.assertEqual(str(uuid.UUID(bytes=bytes(wupts.points[i].id.uuid), version=5)), uuids[i])

if __name__ == '__main__':
    runner = unittest.TextTestRunner(verbosity=3)
    result = runner.run(suite)

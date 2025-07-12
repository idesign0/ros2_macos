# Geographic point, using the WGS 84 reference ellipsoid.

# Latitude [degrees]. Positive is north of equator; negative is south
# (-90.0 <= latitude <= +90.0).
float64 latitude

# Longitude [degrees]. Positive is east of prime meridian; negative is
# west (-180.0 <= longitude <= +180.0). At the poles, latitude is -90.0 or
# +90.0, and longitude is irrelevant, but must be in range.
float64 longitude

# Altitude [m]. Positive is above the WGS 84 ellipsoid (NaN if unspecified).
float64 altitude

### Transforms

##### Rotation

The trace (which is the sum of the diagonal elements in a matrix) is constant independent of the axis, and is computed as:

​			tr(**R**) = 1 + 2cosθ

Rotate an object by an angle around z-axis. Since a rotation around a point is characterized by the fact that the point itself is unaffected by the rotation. We derive the the step like this:

+ Translate the point to the origin
+ Rotate by the z-axis
+ Translate the point back

The equation for column vector is:   
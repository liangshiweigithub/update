### Transforms

##### Rotation

The trace (which is the sum of the diagonal elements in a matrix) is constant independent of the axis, and is computed as:
$$
tr(R) = 1 + 2\cos\theta
$$
Rotate an object by an angle around z-axis. Since a rotation around a point is characterized by the fact that the point itself is unaffected by the rotation. We derive the the step like this:

+ Translate the point to the origin
+ Rotate by the z-axis
+ Translate the point back

The equation for column vector is:  (We use column vector so it is from right to right)
$$
X = T(p)R_{z}(\phi)T(-p)
$$

##### The Rigid-Body Transform

A transform, consisting of concatenations of only translations and rotations, is called a rigid-body transform. A rigid-body matrix can be represented by 
$$
X = T(t)R
$$
where T(t) is translation matrix and R is rotation matrix.  The inverse of X is:
$$
X^{-1} = (T(t)R)^{-1} = R^{-1}T(t)^{-1} = R^TT(-t)
$$
If the matrix is represent like
$$
\left( \begin{array}{cc} 
R & t\\
0^T & 1\\
\end{array} \right)
$$
Then the reverse array is:
$$
X^{-1} = \left( \begin{array}{cccc}
R & -R^Tt\\
0^T & 1
\end{array} \right)
$$

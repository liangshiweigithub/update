#### Spherical Coordinates

In spherical coordinates system, the position of a point or direction and length of a vector defined  by tow angle: $\theta$ and $\phi$. The angle $\theta$ is called **polar angle** and is measured from a fixed zenith direction. The zenith direction in relation to which this polar angle will be measured is the y-axis. (In some reference it is the z-axis.)
$$
x = rsin(\theta)cos(\phi)\\
y=rcos(\theta)\\
z=rsin(\theta)sin(\phi)
$$
Translation from Cartesian coordinate to spherical coordinate:
$$
r = \sqrt{x^2+y^2+z^2}\\
\theta = arccos(\frac{y}{r})\\
\phi =arctan(z, x)
$$
If r equals 1, then
$$
cos\theta = \sqrt{1-x^2-z^2}
$$

#### Differential and Integral Calculus

$$
f'(x) = \lim_{x \to 0} { \dfrac{f(x + \Delta x) - f(x) } {\Delta x}}
$$

Formally, the derivative of the function f at x is the limit of the difference quotient as $\Delta x$ approaches zero. If this limit exists, we then say that $f$ is differentiable at $x$.



https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/mathematics-of-shading?url=mathematics-physics-for-computer-graphics/mathematics-of-shading
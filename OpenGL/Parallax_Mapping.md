Parallax mapping is a lot better in conveying a sense of depth and together with normal mapping gives incredibly realistic results while an illusion. Parallax mapping belongs to the family of **displacement mapping** that displace or offset vertices based on geometrical information stored inside a texture.

One way to do this is to take a plane with roughly 1000 vertices and displace each of these vertices based on a value in a texture that tells us the height of the plane at a specific area. Such a texture that contains height value per texel is called a height map. But this method need extra vertices.

The ideal behind parallax mapping is to alter the texture coordinates in such a way that it looks like a fragment's surface is higher or lower that it actually is, all based on the view direction and a heightmap.

![avatar](../image/parallax_mapping_scaled_height.png)

Parallax mapping aims to offset the texture coordinates at fragment position $A$ in such a way that we get texture coordinates at point $B$ ($\vec P$ points to view direction). When then use the texture coordinates at point $B$ for all subsequent texture samples, making it looks like the viewer is actually looking at point $B$. To get point $B$, parallax mapping tries to solve this by scaling the fragment to view direction vector $\vec V$ by the height at fragment $A$. So we're scaling the length of $\vec V$ to be equal to a sampled value from the heightmap $H(A)$ at the fragment position $A$. We take this vector $\vec P$ and take this vector coordinates that align with the plane as the texture coordinate offset. Issues of this method includes:

+ When the heights change rapidly over the surface the $\vec P$ will not close to $B$.
+ It's difficult to figure out which coordinates to retrieve from $\vec P$ when the surface is arbitrarily rotated in some way. What we'd rather do is parallax mapping in a different coordinate space where the x and y component of vector $\vec P$ always align with the texture's surface. The space is tangent space.
+ 
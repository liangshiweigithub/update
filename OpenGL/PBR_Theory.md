For a PBR lighting model to be considered physically based it has to satisfy the the following conditions:

+ Be based on the microfacet surface model
+ Be energy conserving
+ Use a physically base BRDF

#### The microfacet model

![avatar](..\image\microfacets_light_rays.png)

PBR describes that any surface at a microscopic scale can be described by tiny little perfectly reflective mirrors called microfacets. The rougher a surface is, the more chaotically aligned each microfacet will be along the surface.

The effect of these tiny-like mirror alignments is that when specifically talking about specular lighting/reflection the incoming light rays are more likely to scatter along completely different directions on rougher surfaces, resulting in a more widespread specular reflection.

PBR statistically approximate the surface's microfacet roughness given a **roughness** parameter. Based on the roughness of a surface we can calculate the ratio of microfacets roughly aligned to some vector $h$ (the halfway vector.) **The more the microfacets are aligned to the halfway vector, the sharper and stronger the specular reflection**.

#### Energy conservation

The definition of energy conservation: outgoing light energy should never exceed the incoming light energy (excluding emissive surface). For energy conservation, we need to make a clear distinction between diffuse and specular light. The light gets split in both a refraction part and reflection part when hits a surface. The reflection part get reflected and forms specular lighting. The refraction part is the remaining part that enters the surface and get absorbed. Not all energy is absorbed and the light will be scattered in a random direction which is diffuse light.

**Metallic** surfaces react differently to light compared to non-metallic surfaces. Metallic surfaces follow the same principles of reflection and refraction, but all refracted light gets directly absorbed without scattering, leaving only reflected or specular light; metallic surfaces show no diffuse colors.

#### The reflectance equation

$$
L_0(p, w_0) = \int_{\Omega}^{}f_r(p,w_i,w_0)L_i(p,w_i)n\cdot w_idw_i
$$

Radiance, denoted as $L$, is used to quantify the magnitude or strength of light coming from a single direction.

##### Radiant flux

Radiant flux $\Phi$ is the transmitted energy of a light source measured in Watts.

##### Solid angle: 

The solid angle denoted as $w$ tells us the size or area of a shape projected onto a unit sphere.

##### Radiant intensity

Radiant intensity measures the amount of radiant flux per solid angle or the strength of a light source over a projected area onto the unit sphere. The equation is
$$
I = \frac{d\Phi}{dw}
$$
**Radiance** is described as the total observed energy over an area $A$ over the solid angle $w$ of a light of radiant intensity $\Phi$.

![avatar](../image/radiance.png)

$$
L =\frac{d^2\Phi}{dAdwcos\theta}
$$
Radiance is a radiometric measure of the amount of light in an area scaled by the incident angle $\theta$ of the light to the surface normal. If we consider the solid angle $w$ and area to be infinitely small, we can use radiance to measure the flux of a single ray of light hitting a single point in surface. This relation allows us to calculate the radiance of a single light ray influencing a single point. The solid angle $w$ is translated into a direction vector $w$ and A into a point $p$.

When it comes to radiance we generally care about all incoming light onto a point p which is the sum of all radiance known as irradiance.

$$
L_0(p, w_0) = \int_{\Omega}^{}f_r(p,w_i,w_0)L_i(p,w_i)n\cdot w_idw_i
$$
$L$ in the equation represents the radiance of some point $p$ and some incoming infinitely small solid angle $w_i$ which can be thought of as an incoming direction vector $w_i$. The $cos\theta$ here is represented as $n\cdot w_i$. The reflectance equation calculates the sum of reflected radiance $L_o(p, w_o)$ of a point $p$ in the direction $w_o$ which is the outgoing direction to the viewer. $L_o$ measures the reflected sum of the light's irradiance onto point $p$ as view from $w_o$.

























#### Fresnel reflection

The fresnel equations describes the reflection and transmission of light when incident on an interface between different optical media. The ratio between incident light and reflected light can be calculated by Fresnel reflection.

#### Schlick's approximation

Schlick's approximation is a formula for approximating the contribution of the Fresnel factor in the specular reflection of light. The equation is:
$$
R(\theta) = R_0 + (1-R_0)(1-\vec v \cdot \vec n)^5\\
R_0 = (\frac{n_1-n_2}{n_1+n_2})^2
$$
$n_1, n_2$ are the indices of refraction of the two media at the interface. In computer graphics, one the the interfaces is usually air, meaning that $n_1$ is 1.

In microfacet model it is assumed that there is always a perfect reflection, but the normal changes according to a certain distribution, resulting in a non-perfect overall reflection. When using Schlick's approximation, the normal in the above computation is replaced by the halfway vector.
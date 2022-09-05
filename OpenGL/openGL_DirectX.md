坐标变换中，乘以projection矩阵变换到裁剪空间后，坐标值赋给gl_Position，之后OpenGL自动进行透视除法，转化到标准化设备坐标系(NDC)，然后进行裁剪。

OpenGL的NDC坐标系中，z分量在[-1,1]范围之内，DirectX的z分量在[0,1]之内。

在屏幕映射阶段，NDC坐标被转换为屏幕坐标，同时z分量被映射到[0,1]范围内。屏幕坐标和z坐标一起构成窗口坐标系。OpenGL指定屏幕左下角为(0,0)点，DirectX指定左上角为[0,0]点。



NDC 和屏幕坐标系

https://stackoverflow.com/questions/58702023/what-is-the-coordinate-system-used-in-metal

https://docs.microsoft.com/en-us/windows/win32/api/d3d9helper/nf-d3d9helper-idirect3ddevice9-setviewport

https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glViewport.xhtml

vulkan的ndc：

https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/

ndc 总结

https://zhuanlan.zhihu.com/p/339295068



投影矩阵的推导：

http://www.songho.ca/opengl/gl_projectionmatrix.html

alpha test和alpha blend

https://answer.uwa4d.com/question/59c09367a3d42c411c37a0cd/

https://blog.csdn.net/candycat1992/article/details/41599167

https://www.gamedev.net/forums/topic/656826-why-the-alphablend-is-a-better-choice-than-alphatest-to-implement-transparent-on-mobile-device/

https://zhuanlan.zhihu.com/p/33127345

GPU 架构

https://juejin.cn/post/6844904132864655367

glViewport详解：

https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glViewport.xhtml#:~:text=Description,nd)%20be%20normalized%20device%20coordinates.

glDepthRange:

https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDepthRange.xhtml

depthRange 和 glViewport解析:

https://gamedev.stackexchange.com/questions/8050/in-opengl-how-can-i-discover-the-depth-range-of-a-depth-buffer

Reversed Z：为什么用Reversed Z

https://zhuanlan.zhihu.com/p/341495234

https://zhuanlan.zhihu.com/p/75517534

Linear depth

https://stackoverflow.com/questions/51108596/linearize-depth

https://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer/6657284#6657284

http://web.archive.org/web/20130416194336/http://olivers.posterous.com/linear-depth-in-glsl-for-real

图形学简介：

https://fgiesen.wordpress.com/2011/07/09/a-trip-through-the-graphics-pipeline-2011-index/

metal的 projection matrix：

https://metashapes.com/blog/opengl-metal-projection-matrix-problem/

EVSM:

https://zhuanlan.zhihu.com/p/382202359

metal相关文档。

https://developer.apple.com/documentation/metal



glTexStorage2D and glTexImage2D区别:

https://stackoverflow.com/questions/15405869/is-gltexstorage2d-imperative-when-auto-generating-mipmaps



投影矩阵的推导，一下n和f皆为正值

z映射到[-1, 1]时，投影矩阵如下：
$$
\begin{matrix}
\frac{n}{r} & 0 & 0 & 0\\
0 & \frac{n}{t} & 0 &0\\
0 & 0 & -\frac{f+n}{f-n} & -\frac{2nf}{f-n}\\
0 & 0 & -1 & 0
\end{matrix}
$$
z映射到[0, 1]
$$
\begin{matrix}
\frac{n}{r} & 0 & 0 & 0\\
0 & \frac{n}{t} & 0 &0\\
0 & 0 & -\frac{f}{f-n} & -\frac{fn}{f-n}\\
0 & 0 & -1 & 0
\end{matrix}
$$
z 映射到[1, 0]
$$
\begin{matrix}
\frac{n}{r} & 0 & 0 & 0\\
0 & \frac{n}{t} & 0 &0\\
0 & 0 & \frac{n}{f-n} & \frac{fn}{f-n}\\
0 & 0 & -1 & 0
\end{matrix}
$$

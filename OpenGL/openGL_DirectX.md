坐标变换中，乘以projection矩阵变换到裁剪空间后，坐标值赋给gl_Position，之后OpenGL自动进行透视除法，转化到标准化设备坐标系(NDC)，然后进行裁剪。

OpenGL的NDC坐标系中，z分量在[-1,1]范围之内，DirectX的z分量在[0,1]之内。

在屏幕映射阶段，NDC坐标被转换为屏幕坐标，同时z分量被映射到[0,1]范围内。屏幕坐标和z坐标一起构成窗口坐标系。OpenGL指定屏幕左下角为(0,0)点，DirectX指定左上角为[0,0]点。



NDC 和屏幕坐标系

https://stackoverflow.com/questions/58702023/what-is-the-coordinate-system-used-in-metal

https://docs.microsoft.com/en-us/windows/win32/api/d3d9helper/nf-d3d9helper-idirect3ddevice9-setviewport

https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glViewport.xhtml



投影矩阵的推导：

http://www.songho.ca/opengl/gl_projectionmatrix.html

alpha test和alpha blend

https://answer.uwa4d.com/question/59c09367a3d42c411c37a0cd/

https://blog.csdn.net/candycat1992/article/details/41599167

https://www.gamedev.net/forums/topic/656826-why-the-alphablend-is-a-better-choice-than-alphatest-to-implement-transparent-on-mobile-device/

https://zhuanlan.zhihu.com/p/33127345

GPU 架构

https://juejin.cn/post/6844904132864655367
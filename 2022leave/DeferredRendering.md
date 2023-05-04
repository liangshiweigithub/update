Deferred Rendering:https://zhuanlan.zhihu.com/p/28489928。

https://zhuanlan.zhihu.com/p/618250549

Deferred Rendering最大的优势是将光源数量和场景中物体数目在复杂度层面完全分开，能够渲染很多光源的同时保持很高的帧率。

**步骤：**

+ Geometry Pass: 将所有物体都先绘制到G-buffer。
+ Lighting Pass: 渲染屏幕大小的二维矩形，使用G-buffer中存储的数据对此矩阵的每个片段计算场景光照。

等价于先进行了深度测试，再进行着色计算。复杂度从O(m*n)降到了(n + m);

**优点：**

+ 仅渲染可见像素，节省计算量。
+ 复杂度为O(n+m)，大量光源的场景优势明显
+ 对后处理支持友好

**缺点：**

+ 内存开销大
+ 读写G-buffer的内存带宽是性能瓶颈
+ 透明物体渲染
+ MSAA支持不友好
+ 多种材质支持不友好，只能用同一套lighting pass

**适用于4RT GBuffer**

GBuffer通过stencil标记需要光照的物体，具体为

- B10G11R11F或者RGBA16F （subpass0中存储lightmap间接光照，subpass1中存储最终光照结果）
- RGB10A2（法线）
- RGBA8（BaseColorRGB + 预留）
- RGBA8（Roughness/Metallic/AO/Lightmap Shadow）

针对G-buffer带宽问题，有两种优化手段，分别是

**延迟光照(Deferred Lighting, LightPre-Pass)**

+ 渲染场景中不透明物体，将法线和specular spread factor写入缓冲区。
+ 渲染光照。计算漫反射和镜面反射方程。将结果存到两个RT中。TODO：镜面反射怎么算的？https://www.realtimerendering.com/blog/deferred-lighting-approaches/
+ 对场景中不透明物体进行二次渲染，取漫反射和镜面反射值，对其进行计算，输出结果到最终framebuffer。若没有计算环境光照，则此阶段运用环境光照。

能对每个不同的几何体使用不同的shader进行渲染，因为第三步是forward rendering。缺点是复杂度上来了，需要绘制两遍物体。

**分块延迟渲染 Tile-Based Deferred Rendering**

背景：大量光源下，具体瓶颈将位于每个光源对G-buffer的读取和颜色缓冲区的混合。每个光源即使屏幕空间有重叠，但每个光源不同绘制中进行，会重复读取G-buffer中相同位置的内容。

而分块延迟渲染的主要思想则是把屏幕分拆成细小的栅格，例如每 32 × 32 象素作为一个分块（tile）。然后，计算每个分块会受到哪些光源影响，把那些光源的索引储存在分块的光源列表里。最后，逐个分块进行着色，对每像素读取 G-buffer 和光源列表及相关的光源信息。因此，G-buffer的数据只会被读取1次且仅1次，写入 color buffer也是1次且仅1次，大幅降低内存带宽用量。不过，这种方法需要计算光源会影响哪些分块，这个计算又称为光源剔除（light culling），可以在 CPU 或 GPU（通常以 compute shader 实现）中进行。用GPU计算的好处是，GPU 计算这类工作比 CPU 更快，也减少 CPU／GPU 数据传输。而且，可以计算每个分块的深度范围（depth range），作更有效的剔除。





1. deferred 带宽 ：80M
2. Merge Pass。有逻辑依赖，FB共享
3. backBuffer Size可以修改，决定了UI渲染的精度。
4. GBuffer从7RT到4RT，移动平台最高可能只能支持4RT





性能分析工具：arm streamline

tracy:https://github.com/wolfpld/tracy

arm best practices

https://developer.arm.com/documentation/101897/0300?lang=en

astc low precision解压优化

https://arm-software.github.io/opengl-es-sdk-for-android/astc_textures_low_precision.html

framebuffer fetch

https://www.cnblogs.com/minggoddess/p/11471369.html

pixel local storage的用法

https://www.bilibili.com/read/cv779019/

https://news.16p.com/860415.html

arm vulkan subpass

https://community.arm.com/arm-community-blogs/b/graphics-gaming-and-vr-blog/posts/vulkan-multipass-at-gdc-2017
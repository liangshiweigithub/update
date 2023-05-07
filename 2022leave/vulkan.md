SSBO与UBO的不同之处

1. SSBO更大，OpenGL spec规定UBO是16KB，SSBO可达128MB。
2. SSBO是可以写入的，支持原子操作。写入采用incoherent memory accesses,需要合适的barrier来确保正确性。
3. SSBO支持可变存储，运行期决定block的大小。UBO必须编译期就确定对象的大小。因此SSBO内可有任意长度数组，数组的实际大小基于缓冲区的范围确定。
4. SSBO访问速度更慢。SSBO数据放在显卡的全局内存，UBO放在常量内存中。

https://zhuanlan.zhihu.com/p/20712354。Vulkan设计理念

同步相关

https://zhuanlan.zhihu.com/p/24817959
https://zhuanlan.zhihu.com/p/80692115

https://zhuanlan.zhihu.com/p/25771079

1. Fence

   提供了一种粗粒度的，从Device向Host单向传递信息的机制。Host可以使用Fence来查询通过vkQueueSubmit所提交的操作是否完成。Fence有两种状态，signaled和非signaled状态。

2. Semophore

   用以同步不同的queue之间，或者同一个queue不同的submission之间执行顺序。提供了一种非常灵活的同步queue之间或者queue内部不同command buffer之间的方法。API可以显示的知名不同command buffer之间的资源依赖关系，让driver在遵守依赖关系的前提下，尽可能的最大并行化。

3. Barrier

   Barrier是同一个queue中的command，或者同一个subpass中的command中明确指定的依赖关系。分类有： 

   + Execution Barrier：执行屏障。控制queue中command的执行顺序。不能保证内存访问的顺序。Write-After-Read时使用。不能用于Read-After-Write，因为由于缓存机制，完成写操作后可能没有同步到内存中。

   + Memory Barrier：

     与Execution Barrier相比，可解决无法有效控制缓存问题。有全局，buffer，image三种memory barrier，分别用于不同对象。

   

PSO缓存。

https://docs.unrealengine.com/5.0/en-US/optimizing-rendering-with-pso-caches-in-unreal-engine/

GPU-Driven Rendering Pipelines

https://blog.csdn.net/liran2019/article/details/115618046

Hierarchy Z

https://www.rastergrid.com/blog/2010/10/hierarchical-z-map-based-occlusion-culling/

https://zhuanlan.zhihu.com/p/267179957

SOC

https://km.netease.com/article/299477

https://www.intel.com/content/www/us/en/developer/articles/technical/masked-software-occlusion-culling.html

vulkan samples

https://github.com/KhronosGroup/Vulkan-Samples

ddx, ddy

http://www.aclockworkberry.com/shader-derivative-functions/#footnote_3_1104

https://stackoverflow.com/questions/16365385/explanation-of-dfdx

动态库静态库

https://zhuanlan.zhihu.com/p/71372182

王者优化

https://mp.weixin.qq.com/s/XcsgMKzCplqSu-rm0XVKnQ

水体：

https://mp.weixin.qq.com/s/bPhJcZkS8ISnIBSyWm6jaA
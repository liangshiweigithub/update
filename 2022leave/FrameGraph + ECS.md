## FrameGraph

https://km.netease.com/wiki/561/page/62349?parent_id=9039&wiki=561&has_child=false

1. Legacy pipeline: Immediate mode.命令式过程。

   setRenderTarget()

   setDetphStencil()

   clearFrameBuffer()

   DrawIndexed()

2. Retained mode: 尽可能的把状态传递给下层，更像是一种声明式的编程方式。
   1. 更直观的渲染流程管理，每个renderPass分的很清楚
      + 自带Render target pool
      + 更容易扩展，自定义管线更方便。
   2. 更贴合现代图形API。
   3. 更好的利用On-Chip Memory
      + Correct LOAD/STORE action
      + MemoryLess RT (frameGraph 可以减少图片使用，降低带宽)
      + Subpass / framebuffer fetch
   4. 显示的依赖关系可以更好的并行
      + Multithread encoding
      + Async compute/transfer

3. 执行阶段：

   + Setup：确定需要参与渲染的pass，并指定每个Pass的输入。以lambda的形式传递给FrameGraph

   + Compile：FrameGraph 根据setup指定的依赖顺序一次调用每个pass的setup函数

     + 生成PassTargetNode：Samples，target，Depth资源等
     + 建立PassTargetNode之间的连接关系，cull掉不需要执行的pass。同时确定每个pass执行前需要divirtualized资源和执行完毕后需要destroy的临时资源

   + Execute:  

      对于每一个active PassNode，首先devirtualize它所需要的资源，然后执行相应的逻辑，最后destroy掉它需要destroy的资源。声明周期不重叠的RenderTarget可以被重用

4. RenderTarget的种类:

   + RenderTarget2D: renderTargetPool管理的renderTarget，有缓存机制
   + ExternalRenderTarget：外部的RenderTarget。手动创建和管理，真正持有Device层的RenderTarget对象

5. RenderTarget声明周期：

   + Transient：生命周期一帧之内，可被回收利用
   + 长时间有效，比如ReflectionProbe


## ECS

1. Object Oriented Programming

   用对象来设计程序和应用，对象中包含数据和对应的对象方法。
   
   每个对象自我包含，其中含有代码和数据，定义了接口来访问代码和数据。每个对象可以看成一个黑盒。
   
   + 易维护
   + 可重用
   + 不用理解内部实现就可以使用。
   
3. OO缓存不友好。优化时优先优化数据，然后是代码。内存可能是最大的瓶颈

4. DOD：data oriented design。出现这种设计模式的问题就是CPU和内存间的性能差距越来越大。这种设计模型的核心要求就是尽可能的提升缓存命中率。比如对一个3.2GHz的CPU来说， 从主存中读取数据可能会消耗600个时钟周期，L2 Cache中大概40个时钟周期，L1 Cache中1-2个时钟周期。
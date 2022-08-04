https://miketuritzin.com/post/hierarchical-depth-buffers/

https://www.intel.com/content/www/us/en/developer/articles/technical/masked-software-occlusion-culling.html

https://developer.nvidia.com/gpugems/gpugems2/part-i-geometric-complexity/chapter-6-hardware-occlusion-queries-made-useful

https://developer.nvidia.com/gpugems/gpugems/part-v-performance-and-practicalities/chapter-29-efficient-occlusion-culling

#### 简介：

Hardward occlusion query可以向硬件查询一个物体是否被遮挡，根据查询结果确定是否绘制物体。基本流程如下：

1. 创建一个query
2. 关闭color和depth的写入。
3. 发起query，绘制查询模型的bounding box。
4. 结束query，恢复各种状态。
5. 取得query结果，如果可见则绘制该物体。

但是有两个比较大的问题：

+ 查询有额外开销
+ 需要等待查询结果，从GPU回读数据很慢，CPU和GPU没法并行。

要使用的话，需要解决上面两个问题。首先是要尽可能降低查询的次数，其次是降低查询结果的延迟。

#### 优化方式

1. 利用时间一致性进行适当的可见性猜测。上一帧发出query，下一帧获取结果。即上一帧可见，下一帧也可见，上一帧不可见，下一帧也不可见。猜测错误有一下两种情况：

   + 上一帧可见，本帧不可见，进行了无效处理，并不会影响性能
   + 上一帧不可见，本帧可见，结果错误，需要进行处理。需要本帧进行查询，等待结果。

   另外此种方式没法进行合批操作。因为一次beginQuery和endQuery之间所有的绘制都会当做一个结果返回，没法分drawCall返回可见性。

2. 合并包围盒查询，降低drawcall数量。场景划分层级结构，对于一个节点，如果其子节点全部不可见，则下次查询时合并子节点的查询，只查询此节点。当此节点重新可见时，该帧下绘制次节点下的所有leaf节点，并启动查询，继续进行合并包围盒的操作。

#### 分析：

1. 为了尽可能降低延迟，查询的结果可能要一定帧数之后才可以查询作为当帧的可见性结果，测试中设置为3帧
2. 为了进行合批，需要接受部分结果错误时的情况。即上一帧不可见，本帧可见，正确结果可能要三帧之后才能获得。此外需要复杂的计算方式来合并包围盒的查询。
3. 可能会有硬件兼容性问题
4. 注意使用情形：当drawcall为瓶颈时，会产生负优化，对低端机型不友好
5. 适用于遮挡较多的场景，大片空旷区域可能会导致性能下降

#### 性能

​	在真机实测时，遮挡严重时能有2~10帧的提升，大片空旷区域有帧率下降的情况，2帧左右。

#### TODO

​	降低用来查询的depthBuffer大小，进一步提升性能。
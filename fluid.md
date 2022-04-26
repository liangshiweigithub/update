#### boundaries0:  

输入图：velocity：速度图 

输出图：velocity：速度图

#### advect0: 

输入图：velocity(u)：速度图，velocity(x, the filed to be advected)：速度图
输出图：velocity：速度图

#### boundaries1：

输入图：

density：密度图
输出图：
density：密度图

#### advect1：

输入图：velocity(u, the velovity filed)： 速度图，density(x, the filed to be advected)： 密度图
输出图：density： 密度图。

#### addImpulse:

输入图：velocity(base)： 速度图
输出图：velocity：速度图

#### vorticity：求速度场的旋度

输入图：velocity(u)：速度图
输出图：vorticity: 旋度图

#### vorticityForce: 将求出的旋度作用于速度图

输入图：velocity：速度图，vorticity：旋度图

输出图：速度图

#### diffuse：速度的扩散性，泊松方程, jacobi迭代

输入图：
velocity(x)：速度图,  velocity(b): 速度图
输出图：velocity: 速度图

#### divergence：计算求解出的速度场的散度

输入图：velocity(w)：速度图，
输出图：divergence: 散度图

#### computePressure：泊松方程，jacobi迭代，迭代中夹杂对压力的边界条件

输入图：pressure(x): 压力图，divergence(b)：散度图
输出图：pressure：压力图

#### boundary2：速度图的boundary计算

#### substractGradient：从速度场中减去压力场的梯度：

输入图：pressure(p):压力图，velocity(w):速度图

输出图：velocity：速度图
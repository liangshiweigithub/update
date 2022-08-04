### NeoX到Messiah导出工具

##### 项目背景

​        项目需要在短周期内把基于NeoX引擎的老游戏相关资源和场景导入到Messiah中。老游戏资源老旧，当时制作时资源文件的中间文件(如fbx等文件)已经不存在，美术无法直接将这些文件导入到Messiah中使用。同时组内美术的场景工作流程是在UE中搭建场景，然后导出到Messiah中。老游戏中十多个场景（最后只选定五个场景）重新在UE中搭建然后导出到Messiah中显然会花费大量的美术人力，拖慢项目进度。  

​        在此背景下，独立开发了一套NeoX到Messiah的导出工具。工具将NeoX中模型，地形，场景，动画等各种资源导出到Messiah中，快速的在新引擎中构建了老的场景，节省了大量的美术相关的人力资源。相关导出工作如下：

#### 确定两个引擎坐标系差异

<img src="picture/NeoX坐标系.png" alt="NeoX坐标系" style="zoom:50%;" />

​																				NeoX 坐标系

<img src="picture/Messiah坐标系.png" alt="Messiah坐标系" style="zoom:60%;" />

​      																		Messiah 坐标系

​		NeoX引擎采用的是左手坐标系，Messiah是右手坐标系，坐标系的转换是首要考虑的问题。 确定坐标系差异后，决定需要改变的坐标轴。对于当前两个引擎的坐标系来说，Y和Z轴一一对应后，两坐标系就只有X轴朝向不一致。只要将NeoX坐标系X轴取反即可将两坐标系一一对应起来。对于位置等信息，X坐标简单取反即可。对于旋转相关的计算，简单取反会导致矩阵的负定等问题，具体解决方式后续说明

#### 模型相关资源输出

##### 1. Mesh资源的输出

1. 顶点的位置信息：顶点的位置信息定义于模型空间中。由上可知，两引擎坐标系X轴方向相反。首先根据两个引擎间的缩放比例，对顶点的位置信息进行直接缩放。然后根据坐标系差异，进行变换，将位置信息用Messiah坐标系表示，这样就成功的将模型转换到了Messiah坐标系下。相关计算伪代码如下：

   ```c++
   Vector3 mesiahPos = neoxPos * scale;
   messiahPos.x = -messiahPos.x
   ```

   

2. 法线相关信息：首先根据坐标系差异，进行Normal和Tangent转化，然后叉积方式计算Binormal。相关伪代码如下：

   ```
   tangent.x = -tangent.x;
   normal.x = -normal.x;
   binormal = cross(normal, tangent);
   ```

   

3. 如果三角面环绕顺序不一致，进行三角面环绕顺序的调整。

##### 2. 材质信息输出

 	   策划说明导出后效果没有特殊要求。以前光照计算简单，材质导出简单处理，全部映射为PBR相关材质。

##### 3. 贴图输出

​       NeoX中资源贴图多数为TGA格式，与Messiah相符。读取贴图信息，输出到Messiah即可。对于非TGA格式文件，转化后输出。

#### 地形相关资源输出

​        NeoX以trunk形式管理地形高度数据。Messiah则需要合成整块高度数据。加载NeoX各个地形trunk的高度数据后，根据trunk分布，将高度数据进行合并。

​       合成地形高度数据后，由于两坐标系X轴取向相反，需要调整高度数据。采用的方法是对调高度图X轴方向上的高度信息。然后根据两边地形与实际距离的比例，确定Messiah里输出高度图的尺寸，采用二维差值的方法输出高度图信息。对于控制地表混合权重的权重图，采用相同的方法处理。

#### 场景生成

​        导出资源后，进行场景构建。场景构建涉及到两个引擎下场景中物体的Transform变化。Transform有三部分组成：位置，旋转和缩放。  NeoX中的Transform以矩阵形式提供。读取Transform后，从矩阵中提取位置，缩放，和旋转信息，分别进行处理。

##### 旋转和缩放的处理：

​        对于缩放信息，直接和提前确定的比例相乘即可。对于位置信息，由于两引擎坐标系的差异，首先要进行缩放操作，然后X坐标取反即可。

​		对于旋转，则需要特殊处理。旋转有三种表示方式：欧拉角，矩阵和四元数。

+ 欧拉角用三个角度分别表示绕X,Y,Z轴的旋转角度。欧拉角跟旋转顺序相关，并且会有万向锁问题。如果将提取出来的旋转数据用欧拉角作为中间数据转换，很难确定旋转的顺序，因此首先抛弃以欧拉角作为中间数据的转换方式。

+ 矩阵。旋转也可以用三阶矩阵表示。矩阵的三列分别代表经旋转后的三个轴。由上可知，两引擎在Y，Z两坐标轴对应起来后，只有X轴取向相反。可以将矩阵表示的X轴直接取反，得到新的旋转矩阵。假设原矩阵为：
  $$
  \begin{matrix}
  x_1 & y_1 & z_1\\
  x_2 & y_2 & z_2\\
  x_3 & y_3 & z_3
  \end{matrix}
  $$
  转化后的矩阵变成：
  $$
  \begin{matrix}
  -x_1 & y_1 & z_1\\
  -x_2 & y_2 & z_2\\
  -x_3 & y_3 & z_3
  \end{matrix}
  $$
  用这种方法得出的矩阵，X轴向为负，在Messiah中会带来矩阵的负定问题，所以这种形式的矩阵并不被支持。

+ 四元数形式：

  ![Quaternion](picture/Quaternion.jpg)

  四元数可以理解为绕空间中某一坐标轴旋转特定的角度。假设旋转轴为$\vec u$, 旋转角度为$\theta$, 该旋转的四元数表示为：$\vec q = (u.x * \sin{\theta / 2}, u.y * \sin{\theta/2}, u.z * \sin {\theta/2}, \cos{\theta/2})$。基于此，可以用四元数作为中间结果进行旋转的转换。采用四元数进行旋转变化的具体步骤如下：

1. 将旋转从矩阵表示形式转化为四元数表示形式。
2. 根据四元数表示的旋转意义，从四元数中提取旋转轴，将四元数的旋转轴从NeoX坐标系转到Messiah坐标系来表示。
3. 从四元数中提取旋转角度，根据手系朝向确定Messiah中绕旋转轴顺时针或者逆时针旋转的角度
4. 根据转换后的旋转轴和旋转角度，组合成新的四元数，即用Messiah坐标系表示的四元数。
5. 将四元数转化为用矩阵形式。

伪代码如下所示：

```c++
Quat q = quatCast(rotatationMatrix);
rotationAngle = acos(q.w);
if(rotationAngle > 0.00001)
{
	// 提取旋转轴
	vec3 u = vec3(q.x, q.y, q.z) / sin(rotationAngle);
	//变换旋转轴
	vec3 rotationAxis(-u.x, u.y, u.z);
	// 变化旋转角度
	rotationAngle = -rotationAngle;
	Quat mQuat = Quat(rotationAxis.x / sin(rotationAngle), 
					  rottaionAixs.y / sin(rotationAngle),
					  rotationAixs.z / sin(rotationAngle),
					  cos(rotationAngle));
	Mat4 rotationMatrix = QuatToMatrix(mQuat)
}
```



最后，将变换后的位置，旋转和缩放重新组合，即可得到Messiah中的对应物件的Transform。

#### 动画数据的导出

​        跟模型一样，老游戏中只有NeoX格式下的动画数据，动画编辑无法进行导出操作，并且新游戏中对物件骨骼数量，骨骼名称和骨骼间的架构都有要求，需要特定工具对动画数据进行导出。动画可以分为三要素：蒙皮信息，骨骼架构信息，每帧动画数据。

​		骨骼可以理解为坐标空间，骨骼间存在着嵌套父子关系，骨骼层次关系可以理解为嵌套的坐标系空间。每帧动画数据中存储的是每根骨骼在其父骨骼空间中的Transform。要处理每根骨骼的Transform信息，不能简单的直接转换动画帧里面的数据， 而是要将每个骨骼的位置在其父骨骼空间的坐标系描述转为模型坐标系空间描述，然后再进行数据转换。即对每根骨骼来说，经历的坐标系转换有：

**NeoX Bone space  --> NeoX model space  --> Messiah model space --> Messiah Bone space**

具体转换过程如下所示

1. 加载NeoX中的相关文件，得到骨骼间的父子层级关系和TPos下各个骨骼的位置，根据父子层级关系建立BoneTree。

2. 加载NeoX中的动画数据。

3. 平展动画数据。由于动画数据中骨骼的Transform是基于其父骨骼空间描述的，只有根骨骼的Transform是基于模型空间的，所以需要根据建立好的BoneTree，自下而上一层层进行Transform变化。如果使用column major矩阵，计算过程就是$C = MRootMatrix * ... * MGrandMatrix *MFatherMatrix * MBoneMatrix$.

   计算后可以得到每根骨骼在模型空间中的位置。

4. 得到骨骼在模型空间中Transform后，分别转换位置和缩放信息。对于旋转，通过四元数的方式进行转。得到该骨骼在Messiah坐标系下在模型空间中的位置。转换完后，再将旋转，缩放和位置信息组合为Transform矩阵，即为改骨骼Messiah中模型空间中的位置。

5. 运用同样的处理步骤转换其父骨骼，得到父骨骼在Messiah坐标系下在模型空间中的Transform。

6. 将子骨骼在模型空间中的Transform转换为其在父骨骼空间中的Transform。父骨骼对应的Transform矩阵可将父骨骼空间中的骨骼转换到模型空间中，而我们需要的是将模型空间中的子骨骼转换到父骨骼空间中。所以我们需要的是父骨骼Transform矩阵的逆矩阵将子骨骼从模型空间转到父骨骼空间中，即：

   $targetTransform = inverse(parentBoneModelTrans) * boneTransInModel$

7. 根据项目的骨骼数量和骨骼命名要求，去除冗余骨骼，修改顶点相应的索引，同时重新命名骨骼名称，然后进行输出。

8. skeleton和skin文件转换输出。

​        NeoX到Messiah的导出工具，快速将老引擎中的相关资源导入到了Messiah中。对应场景，美术只需要放置特效，进行lightmap烘焙等微调工作即可。对于模型，不用重新做动画等资源，节省了大量的美术模型相关的人力，导出的场景及动画模型已随H67怀旧服外放。**同时导出中采用的坐标系间Transform转换的方式，可以通用于各种坐标系有差异的编辑器引擎间的资源导入导出**。


### 										Assimp

Assimp is a model importing library. After Assimp has loaded the model, we can retrieve all the data we need from Assimp's data structures. 

![avatar](D:\git-proj\learn_note\image\assimp_structure.png)

+ All the data of the scene/model is contained in the Scene object like all the materials and the meshes. It also contains a reference to the root node of the scene.

+ The root node of the scene may contain children nodes and could have a set of indices that point to mesh data in the scene object's mMeshes array. The mMeshes array of the scene contains the actual Mesh objects, while the mMshes in the Root node contains indices to it.

+ The Mesh object contains all the relevant data required for rendering primitives of object like vertex positions, normal vectors and so on.

+ A Face in mesh represents a render primitive of the object (triangles, squares, points). A face contains the indices of the vertices that form a primitive.

+ Material in the Mesh hosts several functions to retrieve the material properties of an object. 

  ### 									                                        Mesh

See code for detail

### 											Model

```c++
// Code load a model using assimp
Assimp::Importer imported;
const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate|aiProcess_FlipUVs);
```

The first parameter of **Assimp::Importer::ReadFile** is the model file path, while the second is several post-processing options.
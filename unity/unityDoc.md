1. lighting optimization with Precomputed Realtime GI

   https://learn.unity.com/project/lighting-optimization-with-precomputed-realtime-gi

2. static light with light probes

   https://blog.unity.com/technology/static-lighting-with-light-probes

3. **Shader**:
   + **Shader Object**: Instance of Shader class. It Organizes information into structures called **SubShaders and Passes**. It organizes shader program into shader variant.
   + **SubShaders**: contains information about which hard ware, render pipelines, and runtime settings this SubShader is compatible with, SubShader tags which is a key value pairs that provide information about the SubShader. One or more passes.
   + **Passes**: contains pass tag which provide information about the Pass, Instructions for update render state before running its shader programs and shader programs, organized into one or more shader variants.
   + **Shader variant**: The shader programs that a pass contains are organized into shader variants.

4. **ShaderLab**: declarative language that you use in shader source files.

   To define a Shader Object in ShaderLab, use shader block. A typical shader block

   ```c
   Shader "Examples/ShaderSyntax"
   {
       CustomEditor = "ExampleCustomEditor"
   
       Properties
       {
           // Material property declarations go here
       }
       SubShader
       {
           // The code that defines the rest of the SubShader goes here
   
           Pass
           {
              // The code that defines the Pass goes here
           }
       }
   
       Fallback "ExampleFallbackShader"
   }
   ```

   + **Properties:**:  A material property is a property that Unity stores as port of the material assert. This allows artists to create, edit, and share materials with different configurations. Declaration format.

     ```c
     [optioinal: attribute] name("description text", type name) = default val
     ```

     


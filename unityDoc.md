1. **URP Asset**： contains the global rendering and quality settings of your project, and creates the rendering the pipeline instance. The **rendering pipeline instance** contains intermediate resources and the render pipeline implementation.

2. Window->Package manager to **import package**

3. **Render Feature**: an asset that lets you add extra render pass to a URP Renderer and configure their behavior. URP contains pre-built Renderer Feature called **Render Objects**

4. **Render Objects Renderer Feature**:

   URP draws objects in the DrawOpaqueObjects and DrawTransparentObject passes. When you need to draw objects at a different point in the frame rendering, or interpret and write rendering data in alternate ways. The Render Objects Renderer Feature let you do such customizations by letting you draw objects on a certain layer, at certain time, with specific overrides.

5. **Ligting**

   + Baked Global Illumination system consists of **lightmappers**, **Light Probes** , **Reflection Probes**. Three options for baking: The Progressive lightmapper(CPU or GPU), and Enlighten Baked Global Illumination.
   + The real-time Global Illumination system is **Enlighten Realtime Global Illumination**.

   Light Mode: 

   + Realtime
     + Cast shadow up to the shadow distance
     + By default, Realtime Lights contribute only realtime direct lighting to a scene. So shadows appear completely black and there aren't any indirect lighting effects.

   + Baked: used for lighting things that won't change at runtime.

     + bakes both direct and indirect lighting from baked lights into **lightmaps**.
     + bakes both direct and indirect lighting from baked lights into **lightprobes**.

     + do not contribute to specular lighting
     + dynamic object don't receive light or shadow

   + Mixed: Behavior of all Mixed lights depends on the **Lighting mode setting** in the Lighting window.

6. **Skybox**

   SkyManager ensures that environment lighting affects your scene by default. It will generate an **ambient probe** and default **reflection probe** to capture environment lighting. After generate lighting, its probes don't update real timely.

   + Window->Rendering->lighting->Environment: set the default sky material.
   + For a camera specific skybox, add Sky component to the camera, then set the sky material.

7. **Shadow Mapping**: TODO

8. **Lighting Setting Asset: Saved instance of the LightingSettings class**. Which stores data for Baked Global Illumination data and Enlighten Realtime Global Illumination systems data.

9. **Scene**

   **RenderingSettings， lightmapSettings， NavMesh settings， Scene Setting in the occlusion Culling **. Each Scene manages its own settings, so only settings associated with that scene save to the scene file.

   To change the settings of a specific scene, either open that specific scene and change the settings, or **set the scene as the active scene** then change the settings. When switch to a new scene in the Editor or at runtime, Unity replaces all previous settings with the settings from the new active scene.

   + **Shadows and GI light bounces work across all scenes, but the lightmaps and Realtime GI data loads and uploads separately for each scene.** This means scenes don't share and you can unload scenes with lightmaps safely. Scenes do share Light probe data so all light Probes for scenes baked together load at the same time.

10. **Shaders**

    + **Fallback**: if no compatible SubShaders are found, use the named Shader Object. If don't  specify this, use **Fallback Off**.

    + **custom editor**: Use this to display data types that Unity can't display using its default material Inspector, or to define custom controls or data validation.

    + **SubShaders**: Let you define different GPU settings and shader programs for different hardware, render pipelines, and runtime settings. Patterns are like:

      ```
      SubShader
      {
          <optional: LOD>
          <optional: tags>
          <optional: commands>
          <One or more Pass definitions>
      }
      ```

      + **Tags**: Unity uses predefined keys and values to determine how and when to use a given SubShader, or you can create your own custom SubShader tags with custom values. The SubShader tags can be accessed from C# code using the **Material.GetTag**

        + **RenderPipeline Tag**: whether a SubShader is compatible with URP or HDRP. Values: UniversalRenderPipeline, HighDefinitionRenderPipeline.
        + **Queue Tag**: render queue is one of the factors that determines the order that unity renders geometry. Queue tag tells Unity which render queue to use. Values:  **BackGroud, Geometry, AlphaTest, Transparent, overlay**. using **Shader.renderQueue** to read value, **Material.renderQueue, Rendering.RenderQueue** to set the value.
        + **RenderType Tag**: used to override the behavior of a shader object. No preset values.
        + **ForceNoShadowCasting Tag**: no cast shadows.
        + **DisableBatching Tag:** prevent applying dynamic batching to geometry that uses this SubShader. Useful for shader that perform object space operations.
        + **IgnoreProjector Tag**: Built-in pipeline tag.
        + **PreviewType Tag**: tells unity how to display material that uses this SubShader in material Inspector. Signature: "PreviewType" = "[shape]". Shape includes **Sphere, Plane, Skybox**.

      + **LOD Value**

        This value indicates how computationally demanding it is. Inside Shader block, 

        you must put your SubShaders in **descending** LOD order. **Shader.maximumLOD** sets the shader LOD for a given Shader object, **Shader.globalMaximumLOD** sets the LOD for all Shader object.

    + **Pass**: contains instructions for setting the state of the GPU, and the shader programs that run on the GPU. Signature is like:

      ```
      Pass
      {
          <optional: name>
       	<optional: tags>
          <optional: commands>
         	<optional: shader code>
      }
      ```

      + **Tags**: determine how and when to render a given Pass.To access the value of a Pass tag from C# scripts, use the **Shader.FindPassTagValue**.

        + **LightMode Tag**: Predefined tag that Unity uses to determine whether to execute the Pass during a given frame, when during the frame Unity executes the Pass, and what Unity does with the output. URP light mode:

          https://docs.unity3d.com/Packages/com.unity.render-pipelines.universal@11.0/manual/urp-shaders/urp-shaderlab-pass-tags.html#urp-pass-tags-lightmode

11. **ScriptableRenderContext**

    In srp, the ScriptableRenderContext class acts as an interface between the C# render pipeline code and Unity's low-level graphics code. 

    To schedule rendering commands,

    + Pass CommandBuffers to ScriptableRenderContext using ScriptableRenderContext.ExecuteCommandBuffer
    + Make direct API calls to the scriptable Render Context, Such as ScriptableRenderContext.Cull and ScriptableRenderContext.DrawRenderers.

1. Particle Systems:https://docs.unity3d.com/Manual/ChoosingYourParticleSystem.html

2. urp调试：https://zhuanlan.zhihu.com/p/97949167
3. srp源码：https://github.com/Unity-Technologies/Graphics

#### Render Pipeline

1. Pipeline Asset: Specifies which Scriptable Render Pipelines Unity uses and how to configure it. If this is not specified, unity uses the Built-in Render Pipeline. You can create multiple Render Pipeline Asset that use the same render pipeline, but with different configurations.

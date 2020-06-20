### SSAO

In reality, light scatters in all kinds of directions with varying intensities so the indirectly lit parts of the scene should also have varying intensities, instead of a constant ambient component. **Ambient Occlusion** is one type of indirect lighting approximation that tries to approximate indirect lighting by darkening crease, holes and surfaces that are close to each other.

Calculating ambient occlusion techniques are expensive as they have to take surrounding geometry into account. **Screen-space ambient occlusion (SSAO)** uses a scene's depth in screen space to determine the amount of occlusion instead of real geometrical data which is very fast. It is the de-factor standard for approximating real-time ambient occlusion.

The basics of ambient occlusion: For each fragment on a screen-filled quad we calculate an occlusion factor based on the fragment's surrounding depth value. The occlusion factor is then used to reduce or nullify the fragment's ambient lighting component. The occlusion factor is obtained by taking multiple depth samples in a sphere sample kernel surrounding the fragment position and compare each of the samples with the current fragment's depth value. The number of samples that have a higher depth value than the fragment's depth represents the occlusion factor. 

If the sample count is too low the precision drastically reduces and we get an artifact called **banding**. Introducing some randomness into the sample kernel can reduce the amount of samples. By randomly rotating the sample kernel each fragment we can get high quality results with a much smaller amount of samples. The randomness introduces a noticeable noise pattern that can fix by blurring the results.

The sample kernel is **normal oriented hemisphere**. This avoids consider the fragment's underling geometry to be a contribution to the occlusion factor.
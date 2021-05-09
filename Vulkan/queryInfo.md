Occlusion queries works best for large scenes where only a small portion of the scene is visible each frame

If each of the occluded objects is not very complex, then issuing and waiting for queries for all of them is more expensive than just rendering them.

Need a mechanism to group objects close to one another so we can treat them as a single object for occlusion query.

Use temporal coherence. If we know what's visible and what's occluded in one frame, it is very likely that the same classification will be correct for most objects in the following frame as well.





**Issue occlusion queries only for previously visible leaf nodes and for the largest possible occluded nodes in the hierarchy (an occluded node is not tested if its parent is occluded as well).**
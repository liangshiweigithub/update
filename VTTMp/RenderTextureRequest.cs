using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace VirtualTexture
{
    public class RenderTextureRequest
    {
        /// <summary>
        /// x coordinate of the page
        /// </summary>
        public int PageX { get; }

        /// <summary>
        /// y coordinate of the page
        /// </summary>
        public int PageY { get; }

        /// <summary>
        /// mipLevel of the page
        /// </summary>
        public int MipLevel { get; }

        public RenderTextureRequest(int x, int y, int mip)
        {
            PageX = x;
            PageY = y;
            MipLevel = mip;
        }

        public static bool operator == (RenderTextureRequest lhs, RenderTextureRequest rhs)
        {
            return lhs.PageX == rhs.PageX && lhs.PageY == rhs.PageY &&
                lhs.MipLevel == rhs.MipLevel;
        }

        public static bool operator !=(RenderTextureRequest lhs, RenderTextureRequest rhs)
        {
            return !(lhs == rhs);
        }
    }
}

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace VirtualTexture
{
    public class PagePayload
    {
        private static Vector2Int s_InvalidTileIndex = new Vector2Int(-1, -1);

        public Vector2Int TileIndex = s_InvalidTileIndex;

        /// <summary>
        /// �����֡���к�
        /// </summary>
        public int ActiveFrame;

        /// <summary>
        /// ��Ⱦ����
        /// </summary>
        public RenderTextureRequest LoadRequest;

        public bool IsReady {get{ return (TileIndex != s_InvalidTileIndex); } }
        
        /// <summary>
        /// ����ҳ������
        /// </summary>
        public void ResetTileIndex()
        {
            TileIndex = s_InvalidTileIndex;
        }
    }
}

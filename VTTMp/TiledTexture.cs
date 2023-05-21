using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;

namespace VirtualTexture
{
    public class TiledTexture : MonoBehaviour
    {
        /// <summary>
        /// Tile ������ɵ��¼��ص�
        /// </summary>
        public event Action<Vector2> OnTileUpdateComplete;

        /// <summary>
        /// draw tile event
        /// </summary>
        public event Action<RectInt, RenderTextureRequest> DoDrawTexture;

        [SerializeField]
        /// <summary>
        /// ����ߴ磬��������������Tile��������physical Texture��
        /// </summary>
        private Vector2Int m_RegionSize = default;

        [SerializeField]
        private int m_TileSize = 256;

        [SerializeField]
        private int m_PaddingSize = 4;

        private LruCache m_TilePool = new LruCache();

        /// <summary>
        /// tile target texture
        /// </summary>
        public RenderTexture[] VTRTs { get; private set; }


        /// <summary>
        /// ����ߴ磬��������������Tile��������physical Texture��
        /// </summary>
        public Vector2Int RegionSize { get { return m_RegionSize; } }

        /// <summary>
        /// ����Tie�ĳߴ磬 VTRTs�Ĵ�С
        /// </summary>
        public int TileSize { get { return m_TileSize; } }

        /// <summary>
        /// �������ҵ����ߴ磬����֧��Ӳ��������ˣ�Tile����Ч�ߴ��ǣ�TileSize - PaddingSize * 2��
        /// </summary>
        public int PaddingSize { get { return m_PaddingSize; } }

        public int TileSizeWithPadding { get { return TileSize + PaddingSize * 2; } }

        public void Init()
        {
            m_TilePool.Init(RegionSize.x * RegionSize.y);

            VTRTs = new RenderTexture[2];

            VTRTs[0] = new RenderTexture(RegionSize.x * TileSizeWithPadding, RegionSize.y * TileSizeWithPadding, 0);
            VTRTs[0].useMipMap = false;
            VTRTs[0].wrapMode = TextureWrapMode.Clamp;
            Shader.SetGlobalTexture("_VTDiffuse", VTRTs[0]);

            VTRTs[1] = new RenderTexture(RegionSize.x * TileSizeWithPadding, RegionSize.y * TileSize, 0);
            VTRTs[1].useMipMap = false;
            VTRTs[1].wrapMode = TextureWrapMode.Clamp;
            Shader.SetGlobalTexture("_VTNormal", VTRTs[1]);

            // ����Shader����
            // x: paddingƫ����
            // y: tile��Ч����ĳߴ�
            // zw: 1/����ߴ�
            Shader.SetGlobalVector(
                "_VTTileParam",
                new Vector4(
                    (float)PaddingSize,
                    (float)TileSize,
                    RegionSize.x * TileSizeWithPadding,
                    RegionSize.y * TileSizeWithPadding));
        }

        public void Reset()
        {
            // extra set of filterMode
            m_TilePool.Init(RegionSize.x * RegionSize.y);

            VTRTs = new RenderTexture[2];
            VTRTs[0] = new RenderTexture(RegionSize.x * TileSizeWithPadding, RegionSize.y * TileSizeWithPadding, 0);
            VTRTs[0].useMipMap = false;
            VTRTs[0].wrapMode = TextureWrapMode.Clamp;
            VTRTs[0].filterMode = FilterMode.Bilinear;
            Shader.SetGlobalTexture("_VTDiffuse", VTRTs[0]);

            VTRTs[1] = new RenderTexture(RegionSize.x * TileSizeWithPadding, RegionSize.y * TileSizeWithPadding, 0);
            VTRTs[1].useMipMap = false;
            VTRTs[1].wrapMode = TextureWrapMode.Clamp;
            VTRTs[1].filterMode = FilterMode.Bilinear;
            Shader.SetGlobalTexture("_VTNormal", VTRTs[1]);
        }

        public Vector2Int RequestTile()
        {
            return IdToPos(m_TilePool.First);
        }

        public bool SetActive(Vector2Int tile)
        {
            bool success = m_TilePool.SetActive(PosToId(tile));
            return success;
        }

        public void UpdateTile(Vector2Int tile, RenderTextureRequest request)
        {
            if (!SetActive(tile))
                return;

            // RVTTerrain.DrawTexture
            DoDrawTexture?.Invoke(
                new RectInt(tile.x * TileSizeWithPadding, tile.y * TileSizeWithPadding, TileSizeWithPadding, TileSizeWithPadding), 
                request);

            // PageTable.InvalidatePage
            OnTileUpdateComplete?.Invoke(tile);
        } 

        private Vector2Int IdToPos(int id)
        {
            return new Vector2Int(id % RegionSize.x, id / RegionSize.x);
        }

        private int PosToId(Vector2Int tile)
        {
            return (tile.y * RegionSize.x + tile.x);
        }
    }
} // namespace VirtualTexture
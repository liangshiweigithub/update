using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Diagnostics;

namespace VirtualTexture
{
    public class PageTable : MonoBehaviour
    {
        /// <summary>
        /// 页表尺寸
        /// </summary>
        [SerializeField]
        private int m_TableSize = default;


        [SerializeField]
        private Shader m_DebugShader = default;

        [SerializeField]
        private Shader m_DrawLookup = default;

        /// <summary>
        /// 页表层级结构
        /// </summary>
        private PageLevelTable[] m_PageTable;

        private Dictionary<Vector2Int, TableNodeCell> m_ActivePages = new Dictionary<Vector2Int, TableNodeCell>();

        /// <summary>
        /// 页表寻址图
        /// </summary>
        private RenderTexture m_LookupTexture;

        private RenderTextureJob m_RenderTextureJob;

        /// <summary>
        /// 平铺贴图对象，physical texture
        /// </summary>
        private TiledTexture m_TileTexture;

        /// <summary>
        /// 调试材质，显示mipmap等级
        /// </summary>
        private Material m_DebugMaterial;

        public RenderTexture DebugTexture { get; private set; }

        public int TableSize { get { return m_TableSize; } }

        public bool UseFeed { get; set; } = true;

        public int MaxMipLevel { get { return (int)Mathf.Log(TableSize, 2); } }

        private Material drawLookupMat = null;

        private Mesh mQuad;

        public void Init(RenderTextureJob job, int tileCount)
        {
            m_RenderTextureJob = job;
            m_RenderTextureJob.StartRenderJob += OnRenderJob;
            m_RenderTextureJob.CancelRenderJob += OnRenderJobCancel;

            m_LookupTexture = new RenderTexture(TableSize, TableSize, 0);
            m_LookupTexture.filterMode = FilterMode.Point;
            m_LookupTexture.wrapMode = TextureWrapMode.Clamp;

            m_PageTable = new PageLevelTable[MaxMipLevel + 1];
            for (int i = 0; i <= MaxMipLevel; ++i)
            {
                m_PageTable[i] = new PageLevelTable(i, TableSize);
            }
            drawLookupMat = new Material(m_DrawLookup);
            drawLookupMat.enableInstancing = true;

            Shader.SetGlobalTexture("_VTLookupTex", m_LookupTexture);

            Shader.SetGlobalVector("_VTPageParam", new Vector4(TableSize, 1.0f / TableSize, MaxMipLevel, 0));

            InitDebugTexture(TableSize, TableSize);

        }

        private void OnRenderJob(RenderTextureRequest request)
        {
            var node = m_PageTable[request.MipLevel].Get(request.PageX, request.PageY);

            if (node == null || node.Payload.LoadRequest != request)
                return;

            node.Payload.LoadRequest = null;
            var id = m_TileTexture.RequestTile();
            m_TileTexture.UpdateTile(id, request);

            node.Payload.TileIndex = id;
            m_ActivePages[id] = node;
        }

        private void OnRenderJobCancel(RenderTextureRequest request)
        {
            var node = m_PageTable[request.MipLevel].Get(request.PageX, request.PageY);
            if (node == null || node.Payload.LoadRequest != request)
                return;
            node.Payload.LoadRequest = null;
        }

        private void InvalidatePage(Vector2Int id)
        {
            if (!m_ActivePages.TryGetValue(id, out var node))
                return;

            node.Payload.ResetTileIndex();
            m_ActivePages.Remove(id);
        }

        [Conditional("ENABLE_DEBUG_TEXTURE")]
        private void InitDebugTexture(int w, int h)
        {
#if UNITY_EDITOR
            DebugTexture = new RenderTexture(w, h, 0);
            DebugTexture.wrapMode = TextureWrapMode.Clamp;
            DebugTexture.filterMode = FilterMode.Point;
#endif
        }

        [Conditional("ENABLE_DEBUG_TEXTURE")]
        private void UpdateDebugTexture()
        {
#if UNITY_EDITOR
            if (m_LookupTexture == null || m_DebugShader == null)
                return;

            if (m_DebugMaterial == null)
                m_DebugMaterial = new Material(m_DebugShader);

            DebugTexture.DiscardContents();
            Graphics.Blit(m_LookupTexture, DebugTexture, m_DebugMaterial);
#endif
        }    

    }
}
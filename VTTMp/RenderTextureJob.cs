using System;
using System.Collections.Generic;
using UnityEngine;

namespace VirtualTexture
{
    public class RenderTextureJob
    {
        /// <summary>
        /// 渲染完成的回调
        /// </summary>
        public event Action<RenderTextureRequest> StartRenderJob;

        /// <summary>
        /// 渲染取消的回调
        /// </summary>
        public event Action<RenderTextureRequest> CancelRenderJob;

        [SerializeField]
        private int m_Limit = 2;

        private List<RenderTextureRequest> m_PendingRequests = new List<RenderTextureRequest>();
    
        RenderTextureRequest Request(int x, int y, int mip)
        {
            foreach(var r in m_PendingRequests)
            {
                if(r.PageX == x && r.PageY == y && r.MipLevel == mip)
                {
                    return null;
                }
            }

            var request = new RenderTextureRequest(x, y, mip);
            m_PendingRequests.Add(request);
            return request;
        }

        public void ClearJob()
        {
            foreach(var r in m_PendingRequests)
            {
                CancelRenderJob?.Invoke(r);
            }
            m_PendingRequests.Clear();
        }

        public void Update()
        {
            if (m_PendingRequests.Count == 0)
                return;

            m_PendingRequests.Sort((x, y) => { return x.MipLevel.CompareTo(y.MipLevel); });
            int count = m_Limit;
            while(count > 0 && m_PendingRequests.Count > 0)
            {
                count--;
                var req = m_PendingRequests[m_PendingRequests.Count - 1];
                m_PendingRequests.RemoveAt(m_PendingRequests.Count - 1);

                // PageTable.OnRenderJob
                StartRenderJob?.Invoke(req);
            }
        }
    }
}
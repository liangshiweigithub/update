using System.Collections;
using System.Collections.Generic;
using UnityEngine;


namespace VirtualTexture
{
    public class LruCache
    {
        public class NodeInfo
        {
            public int id = 0;
            public NodeInfo Next { get; set; }
            public NodeInfo Prev { get; set; }
        }

        private NodeInfo[] allNodes;

        private NodeInfo head = null;
        private NodeInfo tail = null;

        public int First { get { return head.id; } }

        public void Init(int count)
        {
            allNodes = new NodeInfo[count];
            for(int i = 0; i < count; ++i)
            {
                allNodes[i] = new NodeInfo() { id = i, };
            }

            for(int i = 0; i < count; ++i)
            {
                allNodes[i].Prev = i == 0 ? null : allNodes[i - 1];
                allNodes[i].Next = (i + 1 < count ? allNodes[i + 1] : null); 
            }
            head = allNodes[0];
            tail = allNodes[count - 1];
        }

        public bool SetActive(int id)
        {
            if (id < 0 || id >= allNodes.Length)
                return false;
            if (id == tail.id)
                return true;

            var node = allNodes[id];
            RemoveNode(node);
            AddLast(node);
            return true;
        }

        void AddLast(NodeInfo node)
        {
            NodeInfo last = tail;
            last.Next = node;
            node.Prev = last;
            tail = node;
        }

        void RemoveNode(NodeInfo node)
        {
            if (head == node)
            {
                head = head.Next;
            }
            else
            {
                node.Prev.Next = node.Next;
                node.Next.Prev = node.Prev;
            }
        }

    }
}
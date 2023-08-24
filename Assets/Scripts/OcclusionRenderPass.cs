using System;
using System.Runtime.InteropServices;
using UnityEngine.Rendering;
using UnityEngine.Rendering.Universal;

public class OcclusionRenderPass : ScriptableRenderPass
{
#if (UNITY_IOS || UNITY_TVOS || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport ("__Internal")]
#else
    [DllImport("OcclusionQueriesPlugin")]
#endif
    private static extern IntPtr GetRenderEventFunc();

    private const int TEST_EVENT_ID = 0;
    private const int PREPARE_API_EVENT_ID = 1;

    private readonly IntPtr m_RenderEventFunc;
    private readonly ProfilingSampler m_ProfilingSampler = new ProfilingSampler("Occlusion Queries");

    public OcclusionRenderPass()
    {
        m_RenderEventFunc = GetRenderEventFunc();
    }

    public override void Execute(ScriptableRenderContext context, ref RenderingData renderingData)
    {
        CommandBuffer cmd = CommandBufferPool.Get("Occlusion Queries");
        context.ExecuteCommandBuffer(cmd);
        cmd.Clear();

        cmd.IssuePluginEvent(m_RenderEventFunc, PREPARE_API_EVENT_ID);

        using (new ProfilingScope(cmd, m_ProfilingSampler))
        {
            cmd.IssuePluginEvent(m_RenderEventFunc, TEST_EVENT_ID);
        }

        context.ExecuteCommandBuffer(cmd);
        cmd.Clear();

        CommandBufferPool.Release(cmd);
    }
}
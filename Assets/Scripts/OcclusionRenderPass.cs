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

    private readonly ProfilingSampler m_ProfilingSampler = new ProfilingSampler("Occlusion Queries");

    public override void Execute(ScriptableRenderContext context, ref RenderingData renderingData)
    {
        CommandBuffer cmd = CommandBufferPool.Get("Occlusion Queries");
        context.ExecuteCommandBuffer(cmd);
        cmd.Clear();

        using (new ProfilingScope(cmd, m_ProfilingSampler))
        {
            cmd.IssuePluginEvent(GetRenderEventFunc(), 0);
        }

        context.ExecuteCommandBuffer(cmd);
        cmd.Clear();

        CommandBufferPool.Release(cmd);
    }
}
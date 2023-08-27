using System;
using System.Runtime.InteropServices;
using Unity.Collections;
using Unity.Mathematics;
using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.Rendering.Universal;

public class OcclusionRenderPass : ScriptableRenderPass, IDisposable
{
#if (UNITY_IOS || UNITY_TVOS || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport ("__Internal")]
#else
    [DllImport("OcclusionQueriesPlugin")]
#endif
    private static extern IntPtr GetRenderEventFunc();

#if (UNITY_IOS || UNITY_TVOS || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport ("__Internal")]
#else
    [DllImport("OcclusionQueriesPlugin")]
#endif
    private static extern IntPtr GetRenderEventWithDataFunc();

    private const int RENDER_EVENT_ID = 0;
    private const int PREPARE_API_EVENT_ID = 1;

    private readonly IntPtr m_RenderEventFunc;
    private readonly IntPtr m_RenderEventWithDataFunc;
    private readonly IntPtr m_MatricesBufferNativePtr;

    private GraphicsBuffer m_MatricesBuffer;
    private NativeArray<float4x4> m_MatricesArray;

    private readonly ProfilingSampler m_ProfilingSampler = new ProfilingSampler("Occlusion Queries");

    public OcclusionRenderPass()
    {
        m_RenderEventFunc = GetRenderEventFunc();
        m_RenderEventWithDataFunc = GetRenderEventWithDataFunc();

        m_MatricesArray = new NativeArray<float4x4>(1, Allocator.Persistent);
        m_MatricesBuffer = new GraphicsBuffer(GraphicsBuffer.Target.Constant, m_MatricesArray.Length, 16 * 4);
        m_MatricesBuffer.SetData(m_MatricesArray); // set data to get correct native ptr
        m_MatricesBufferNativePtr = m_MatricesBuffer.GetNativeBufferPtr();
    }

    public override void Execute(ScriptableRenderContext context, ref RenderingData renderingData)
    {
        CommandBuffer cmd = CommandBufferPool.Get("Occlusion Queries");
        context.ExecuteCommandBuffer(cmd);
        cmd.Clear();

        float4x4 vpMatrix = renderingData.cameraData.GetProjectionMatrix() * renderingData.cameraData.GetViewMatrix();

        cmd.IssuePluginEventAndData(m_RenderEventWithDataFunc, PREPARE_API_EVENT_ID, m_MatricesBufferNativePtr);

        using (new ProfilingScope(cmd, m_ProfilingSampler))
        {
            m_MatricesArray[0] = math.mul(vpMatrix, float4x4.identity);
            cmd.SetBufferData(m_MatricesBuffer, m_MatricesArray);
            cmd.IssuePluginEvent(m_RenderEventFunc, RENDER_EVENT_ID);
        }

        context.ExecuteCommandBuffer(cmd);
        cmd.Clear();

        CommandBufferPool.Release(cmd);
    }

    public void Dispose()
    {
        m_MatricesArray.Dispose();
        m_MatricesBuffer?.Release();
        m_MatricesBuffer = null;
    }
}
using UnityEngine;
using UnityEngine.Rendering.Universal;

public class OcclusionRendererFeature : ScriptableRendererFeature
{
    private OcclusionRenderPass m_Pass;

    public override void Create()
    {
        m_Pass = new OcclusionRenderPass();
    }

    public override void AddRenderPasses(ScriptableRenderer renderer, ref RenderingData renderingData)
    {
        if (m_Pass != null)
        {
            renderer.EnqueuePass(m_Pass);
        }
    }

    protected override void Dispose(bool disposing)
    {
        m_Pass?.Dispose();
        m_Pass = null;
    }
}
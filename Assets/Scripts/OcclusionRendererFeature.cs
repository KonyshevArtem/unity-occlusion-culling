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
        if (renderingData.cameraData.cameraType == CameraType.Game)
        {
            renderer.EnqueuePass(m_Pass);
        }
    }
}
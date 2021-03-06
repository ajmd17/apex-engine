#include "pssm_shadow_mapping.h"

namespace apex {
PssmShadowMapping::PssmShadowMapping(Camera *view_cam, int num_splits, int max_dist)
    : num_splits(num_splits)
{
    shadow_renderers.resize(num_splits, nullptr);

    Environment::GetInstance()->SetNumCascades(num_splits);

    double max_distance = (double)max_dist;
    const double min_dist = max_distance / (double)num_splits;

    for (int i = num_splits - 1; i >= 0; i--) {
        const double distance = MathUtil::Lerp(min_dist, max_distance, (double)(i) / ((double)num_splits - 1));
        Environment::GetInstance()->SetShadowSplit(i, distance);
        shadow_renderers[i] = std::make_shared<ShadowMapping>(view_cam, distance);

        Environment::GetInstance()->SetShadowMap(i, shadow_renderers[i]->GetShadowMap());
    }
}

int PssmShadowMapping::NumSplits() const
{
    return num_splits;
}

void PssmShadowMapping::SetLightDirection(const Vector3 &dir)
{
    for (int i = 0; i < num_splits; i++) {
        shadow_renderers[i]->SetLightDirection(dir);
    }
}

void PssmShadowMapping::Render(Renderer *renderer)
{
    for (int i = 0; i < num_splits; i++) {
        shadow_renderers[i]->Begin();
        CoreEngine::GetInstance()->Clear(GL_DEPTH_BUFFER_BIT);

        Environment::GetInstance()->SetShadowMatrix(i, shadow_renderers[i]->
            GetShadowCamera()->GetViewProjectionMatrix());

        renderer->RenderBucket(shadow_renderers[i]->GetShadowCamera(), renderer->opaque_bucket);

        shadow_renderers[i]->End();
    }
}
} // namespace apex

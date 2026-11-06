//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

// The dataset, read and written in place. Bound as a storage buffer through a
// push descriptor, matching the layout built by al::ComputePipeline.
[[vk::binding(0, 0)]]
RWStructuredBuffer<uint> data;

// The push constants, matching the range built by al::ComputePipeline.
struct PushConstants
{
    uint count; ///< The number of elements in the dataset.
};

[[vk::push_constant]]
ConstantBuffer<PushConstants> pc;

[shader("compute")]
[numthreads(64, 1, 1)]
void computeMain(uint3 dtid: SV_DispatchThreadID)
{
    const uint i = dtid.x;
    if (i >= pc.count)
    {
        return; // The dispatch is rounded up to whole groups, so trim the tail.
    }

    data[i] = data[i] * 2;
}

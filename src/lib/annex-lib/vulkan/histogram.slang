//
// Copyright (c) 2026 Jamie Kenyon. All Rights Reserved.
//

RWStructuredBuffer<uint> keys;
RWStructuredBuffer<uint> hist;
uniform uint count, shift, numGroups;
groupshared uint localHist[256];

[shader("compute")]
[numthreads(256, 1, 1)]
void computeMain(uint3 gid : SV_GroupID, uint tid : SV_GroupIndex)
{
    localHist[tid] = 0;
    GroupMemoryBarrierWithGroupSync();
    uint i = gid.x * 256 + tid;
    if (i < count)
        InterlockedAdd(localHist[(keys[i] >> shift) & 0xFF], 1);
    GroupMemoryBarrierWithGroupSync();
    hist[tid * numGroups + gid.x] = localHist[tid];
}

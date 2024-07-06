dxc -T vs_6_0 -E Main -spirv -Fo DefaultVertex.spv DefaultVertex.hlsl
dxc -T ps_6_0 -E Main -spirv -Fo DefaultFragment.spv DefaultFragment.hlsl

pause
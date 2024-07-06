dxc -T vs_6_0 -E main -spirv -Fo DefaultVertex.spv DefaultVertex.hlsl
dxc -T ps_6_0 -E main -spirv -Fo DefaultFragment.spv DefaultFragment.hlsl

pause
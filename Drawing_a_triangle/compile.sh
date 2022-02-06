COMPILER="/Users/toyotariku/VulkanSDK/1.2.198.1/macOS/bin/glslc"
SHADERDIR="./src/shader"
OUTPUTDIR="./spv"
$COMPILER $SHADERDIR/shader.vert -o $OUTPUTDIR/vert.spv
$COMPILER $SHADERDIR/shader.frag -o $OUTPUTDIR/frag.spv
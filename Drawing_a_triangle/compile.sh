OS="Ubuntu" #"Mac"
if [ $OS = "Mac" ]; then
    COMPILER="/Users/toyotariku/VulkanSDK/1.2.198.1/macOS/bin/glslc"
else
    COMPILER="/home/honolulu/programs/downloaded-libraries/vulkan1.3.204.0/x86_64/bin/glslc"
fi
SHADERDIR="./src/shader"
mkdir -p spv
OUTPUTDIR="./spv"
$COMPILER $SHADERDIR/shader.vert -o $OUTPUTDIR/vert.spv
$COMPILER $SHADERDIR/shader.frag -o $OUTPUTDIR/frag.spv
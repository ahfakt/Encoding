# Encoding

```shell
git clone https://github.com/ahfakt/Stream.git
git clone https://github.com/ahfakt/Encoding.git

# Debug, Release, RelWithDebInfo, MinSizeRel
BUILD_TYPE='Release'

# Configure
cmake \
    -D "CMAKE_BUILD_TYPE:STRING=${BUILD_TYPE}" \
    -S 'Encoding' \
    -B "build/${BUILD_TYPE}/Encoding" \
    -G "Unix Makefiles"

# Build
cmake \
    --build "build/${BUILD_TYPE}/Stream" \
    --config "${BUILD_TYPE}" \
    --target all
```
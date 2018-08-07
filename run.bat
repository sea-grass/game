if not exist .emscripten_cache mkdir .emscripten_cache
if not exist build mkdir build
docker run --rm -v %cd%:/src -v %cd%/.emscripten_cache:/root/.emscripten_cache -t apiaryio/emcc emcc -s LEGACY_GL_EMULATION=1 -s USE_PTHREADS=1 main.c shape.c shapeList.c -o build/hello_world.html --post-js src/preRun.js
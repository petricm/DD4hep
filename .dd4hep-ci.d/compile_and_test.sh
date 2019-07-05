#!/bin/bash

source /DD4hep/.dd4hep-ci.d/init_lcg.sh && \
cd /DD4hep && \
mkdir build && \
cd build && \
cmake -GNinja -DDD4HEP_USE_GEANT4=ON -DBoost_NO_BOOST_CMAKE=ON -DDD4HEP_USE_LCIO=OFF -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release -DROOT_DIR=$ROOTSYS -DCMAKE_CXX_FLAGS="-fdiagnostics-color=always -Werror" -DCMAKE_CXX_STANDARD=17 .. && \
ninja && \
ninja install && \
. ../bin/thisdd4hep.sh && \
ctest --output-on-failure -j4 && \
cd ../examples/ && \
mkdir build && \
cd build && \
cmake -GNinja -DBoost_NO_BOOST_CMAKE=ON .. && \
ninja && \
ninja install && \
ctest --output-on-failure -j2

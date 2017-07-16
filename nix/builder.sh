source $setup

cmake-cross $src \
  -DBUILD_SHARED_LIBS=false \
  -DCMAKE_INSTALL_PREFIX=$out

make
make install
$host-strip $out/bin/*
cp version.txt $out/

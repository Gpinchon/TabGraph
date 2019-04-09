cd ./libs/gdal/gdal
./autogen.sh
./configure \
    --prefix=${PREFIX} \
    --with-hide-internal-symbols \
    --with-geos \
    --with-geotiff=internal \
    --with-libtiff=internal \
    --with-libz=internal \
    --with-libjson-c=internal \
    --with-threads \
    --without-bsb \
    --without-cfitsio \
    --without-cryptopp \
    --without-curl \
    --without-dwgdirect \
    --without-ecw \
    --without-expat \
    --without-fme \
    --without-freexl \
    --without-gif \
    --without-gif \
    --without-gnm \
    --without-grass \
    --without-grib \
    --without-hdf4 \
    --without-hdf5 \
    --without-idb \
    --without-ingres \
    --without-jasper \
    --without-jp2mrsid \
    --without-jpeg \
    --without-kakadu \
    --without-libgrass \
    --without-libkml \
    --without-libtool \
    --without-mrf \
    --without-mrsid \
    --without-mysql \
    --without-netcdf \
    --without-odbc \
    --without-ogdi \
    --without-openjpeg \
    --without-pcidsk \
    --without-pcraster \
    --without-pcre \
    --without-perl \
    --without-pg \
    --without-php \
    --without-png \
    --without-python \
    --without-qhull \
    --without-sde \
    --without-sqlite3 \
    --without-webp \
    --without-xerces \
    --without-xml2
/bin/make -j
mkdir ./tmpobj/
ld -r ./frmts/o/*.o             -o ./tmpobj/frmts.o
ld -r ./gcore/*.o               -o ./tmpobj/core.o
ld -r ./port/*.o                -o ./tmpobj/port.o
ld -r ./alg/*.o                 -o ./tmpobj/alg.o
ld -r ./apps/*.o                -o ./tmpobj/apps.o
ld -r ./ogr/ogrsf_frmts/o/*.o   -o ./tmpobj/ogrsf_frmts.o
ld -r ./third_party/o/*.o       -o ./tmpobj/third_party.o
ld -r ./ogr/*.o                 -o ./tmpobj/ogr.o
ar r ./libgdal.a ./tmpobj/frmts.o ./tmpobj/core.o ./tmpobj/port.o ./tmpobj/alg.o ./tmpobj/apps.o ./tmpobj/ogrsf_frmts.o ./tmpobj/third_party.o ./tmpobj/ogr.o
ranlib ./libgdal.a
rm -rf ./tmpobj/
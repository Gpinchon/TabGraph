cd ./libs/gdal/gdal
./autogen.sh
./configure \
    --prefix=${PREFIX}              \
    --with-cpp14                    \
    --with-hide-internal-symbols    \
    --with-threads                  \
    --with-liblzma=no               \
    --with-libz=internal            \
    --with-pcraster=internal        \
    --with-png=internal             \
    --with-pcidsk=internal          \
    --with-libtiff=internal         \
    --with-geotiff=internal         \
    --with-jpeg=internal            \
    --with-gif=internal             \
    --with-qhull=internal           \
    --with-libjson-c=internal       \
    --without-cfitsio               \
    --without-cryptopp              \
    --without-curl                  \
    --without-ecw                   \
    --without-expat                 \
    --without-fme                   \
    --without-freexl                \
    --without-gnm                   \
    --without-grass                 \
    --without-hdf4                  \
    --without-hdf5                  \
    --without-idb                   \
    --without-ingres                \
    --without-jasper                \
    --without-jp2mrsid              \
    --without-kakadu                \
    --without-libgrass              \
    --without-libkml                \
    --without-libtool               \
    --without-mrsid                 \
    --without-mysql                 \
    --without-netcdf                \
    --without-odbc                  \
    --without-ogdi                  \
    --without-openjpeg              \
    --without-pcre                  \
    --without-perl                  \
    --without-pg                    \
    --without-python                \
    --without-sde                   \
    --without-sqlite3               \
    --without-webp                  \
    --without-xerces                \
    --without-xml2                  \
    --without-freexl                \
    --without-poppler               \
    --without-podofo                \
    --without-pdfium                \
    --without-hdfs                  \
    --without-armadillo             \
    --without-crypto                \
    --without-ld-shared             \
    --disable-shared                \
    --enable-static

rm -f ./libgdal.a
make -j 4
mkdir ./tmpobj/

ld -r -S -o ./tmpobj/basepart.o     ./frmts/o/*.o ./gcore/*.o ./port/*.o ./alg/*.o 
ld -r -S -o ./tmpobj/apps.o         ./apps/commonutils.o ./apps/gdalinfo_lib.o ./apps/gdal_translate_lib.o ./apps/gdalwarp_lib.o ./apps/ogr2ogr_lib.o ./apps/gdaldem_lib.o ./apps/nearblack_lib.o ./apps/gdal_grid_lib.o ./apps/gdal_rasterize_lib.o ./apps/gdalbuildvrt_lib.o 
ld -r -S -o ./tmpobj/ogrsf_frmts.o  ./ogr/ogrsf_frmts/o/*.o 
ld -r -S -o ./tmpobj/third_party.o  ./third_party/o/*.o
ld -r -S -o ./tmpobj/ogr.o          ./ogr/ogrgeometryfactory.o ./ogr/ogrpoint.o ./ogr/ogrcurve.o ./ogr/ogrlinestring.o ./ogr/ogrlinearring.o ./ogr/ogrpolygon.o ./ogr/ogrtriangle.o ./ogr/ogrutils.o ./ogr/ogrgeometry.o ./ogr/ogrgeometrycollection.o ./ogr/ogrmultipolygon.o ./ogr/ogrsurface.o ./ogr/ogrpolyhedralsurface.o ./ogr/ogrtriangulatedsurface.o ./ogr/ogrmultipoint.o ./ogr/ogrmultilinestring.o ./ogr/ogrcircularstring.o ./ogr/ogrcompoundcurve.o ./ogr/ogrcurvepolygon.o ./ogr/ogrcurvecollection.o ./ogr/ogrmulticurve.o ./ogr/ogrmultisurface.o ./ogr/ogr_api.o ./ogr/ogrfeature.o ./ogr/ogrfeaturedefn.o ./ogr/ogrfeaturequery.o ./ogr/ogrfeaturestyle.o ./ogr/ogrfielddefn.o ./ogr/ogrspatialreference.o ./ogr/ogr_srsnode.o ./ogr/ogr_fromepsg.o ./ogr/ogrct.o ./ogr/ogr_srs_esri.o ./ogr/ogr_srs_pci.o ./ogr/ogr_srs_usgs.o ./ogr/ogr_srs_dict.o ./ogr/ogr_srs_panorama.o ./ogr/ogr_srs_ozi.o ./ogr/ogr_srs_erm.o ./ogr/swq.o ./ogr/swq_expr_node.o ./ogr/swq_parser.o ./ogr/swq_select.o ./ogr/swq_op_registrar.o ./ogr/swq_op_general.o ./ogr/ogr_srs_xml.o ./ogr/ograssemblepolygon.o ./ogr/ogr2gmlgeometry.o ./ogr/gml2ogrgeometry.o ./ogr/ogr_expat.o ./ogr/ogrpgeogeometry.o ./ogr/ogrgeomediageometry.o ./ogr/ogr_geocoding.o ./ogr/ogrgeomfielddefn.o ./ogr/ograpispy.o ./ogr/ogr_xerces.o ./ogr/ogr_geo_utils.o ./ogr/ogr_proj_p.o                                                                                            


#ld -r -S -o ./tmpobj/basepart.o ./frmts/o/*.o ./gcore/*.o ./port/*.o ./alg/*.o
#ld -r -S -o ./tmpobj/apps.o \
#        ./apps/commonutils.o        \
#        ./apps/gdalinfo_lib.o       \
#        ./apps/gdal_translate_lib.o \
#        ./apps/gdalwarp_lib.o       \
#        ./apps/ogr2ogr_lib.o        \
#        ./apps/gdaldem_lib.o        \
#        ./apps/nearblack_lib.o      \
#        ./apps/gdal_grid_lib.o      \
#        ./apps/gdal_rasterize_lib.o \
#        ./apps/gdalbuildvrt_lib.o
#ld -r -S -o ./tmpobj/ogrsf_frmts.o \
#        ./ogr/ogrsf_frmts/o/*.o
#ld -r -S -o ./tmpobj/third_party.o \
#        ./third_party/o/*.o
#ld -r -S -o ./tmpobj/ogr.o \
#        ./ogr/ogrgeometryfactory.o \
#        ./ogr/ogrpoint.o \
#        ./ogr/ogrcurve.o \
#        ./ogr/ogrlinestring.o \
#        ./ogr/ogrlinearring.o \
#        ./ogr/ogrpolygon.o \
#        ./ogr/ogrtriangle.o \
#        ./ogr/ogrutils.o \
#        ./ogr/ogrgeometry.o \
#        ./ogr/ogrgeometrycollection.o \
#        ./ogr/ogrmultipolygon.o \
#        ./ogr/ogrsurface.o \
#        ./ogr/ogrpolyhedralsurface.o \
#        ./ogr/ogrtriangulatedsurface.o \
#        ./ogr/ogrmultipoint.o \
#        ./ogr/ogrmultilinestring.o \
#        ./ogr/ogrcircularstring.o \
#        ./ogr/ogrcompoundcurve.o \
#        ./ogr/ogrcurvepolygon.o \
#        ./ogr/ogrcurvecollection.o \
#        ./ogr/ogrmulticurve.o \
#        ./ogr/ogrmultisurface.o \
#        ./ogr/ogr_api.o \
#        ./ogr/ogrfeature.o \
#        ./ogr/ogrfeaturedefn.o \
#        ./ogr/ogrfeaturequery.o \
#        ./ogr/ogrfeaturestyle.o \
#        ./ogr/ogrfielddefn.o \
#        ./ogr/ogrspatialreference.o \
#        ./ogr/ogr_srsnode.o \
#        ./ogr/ogr_fromepsg.o \
#        ./ogr/ogrct.o \
#        ./ogr/ogr_srs_esri.o \
#        ./ogr/ogr_srs_pci.o \
#        ./ogr/ogr_srs_usgs.o \
#        ./ogr/ogr_srs_dict.o \
#        ./ogr/ogr_srs_panorama.o \
#        ./ogr/ogr_srs_ozi.o \
#        ./ogr/ogr_srs_erm.o \
#        ./ogr/swq.o \
#        ./ogr/swq_expr_node.o \
#        ./ogr/swq_parser.o \
#        ./ogr/swq_select.o \
#        ./ogr/swq_op_registrar.o \
#        ./ogr/swq_op_general.o \
#        ./ogr/ogr_srs_xml.o \
#        ./ogr/ograssemblepolygon.o \
#        ./ogr/ogr2gmlgeometry.o \
#        ./ogr/gml2ogrgeometry.o \
#        ./ogr/ogr_expat.o \
#        ./ogr/ogrpgeogeometry.o \
#        ./ogr/ogrgeomediageometry.o \
#        ./ogr/ogr_geocoding.o \
#        ./ogr/ogrgeomfielddefn.o \
#        ./ogr/ograpispy.o \
#        ./ogr/ogr_xerces.o \
#        ./ogr/ogr_geo_utils.o \
#        ./ogr/ogr_proj_p.o

ar -rs ./libgdal.a ./tmpobj/basepart.o ./tmpobj/apps.o ./tmpobj/ogrsf_frmts.o ./tmpobj/third_party.o ./tmpobj/ogr.o
rm -rf ./tmpobj/

#--with-cpp14                                    #Enable C++14 compiler options
#--with-pic[=PKGS]                               #try to use only PIC/non-PIC objects [default=use
#--with-aix-soname=aix|svr4|both                 
#--with-gnu-ld                                   #assume the C compiler uses GNU ld [default=no]
#--with-sysroot[=DIR]                            #Search for dependent libraries within DIR (or the
#--without-libtool                               #Don't use libtool to build the library
#--without-ld-shared                             #Disable shared library support
#--with-unix-stdio-64=ARG                        #Utilize 64 stdio api (yes/no)
#--with-sse=ARG                                  #Detect SSE availability for some optimized routines (ARG=yes(default), no)
#--with-ssse3=ARG                                #Detect SSSE3 availability for some optimized routines (ARG=yes(default), no)
#--with-avx=ARG                                  #Detect AVX availability for some optimized routines (ARG=yes(default), no)
#--with-hide-internal-symbols=ARG                #Try to hide internal symbols (ARG=yes/no)
#--with-rename-internal-libtiff-symbols=ARG      #Prefix internal libtiff symbols with gdal_ (ARG=yes/no)
#--with-rename-internal-libgeotiff-symbols=ARG   #Prefix internal libgeotiff symbols with gdal_ (ARG=yes/no)
#--with-rename-internal-shapelib-symbols=ARG     #Prefix internal shapelib symbols with gdal_ (ARG=yes/no)
#--with-local=dir                                #Include /usr/local or other local tree for INCLUDE/LIBS
#--with-threads=ARG                              #Include thread safe support (ARG=yes(default), no or linkopt)
#--with-libz=ARG                                 #Include libz support (ARG=internal or libz directory)
#--with-gnu-ld                                   #assume the C compiler uses GNU ld default=no
#--with-libiconv-prefix[=DIR]                    #search for libiconv in DIR/include and DIR/lib
#--without-libiconv-prefix                       #don't search for libiconv in includedir and libdir
#--with-proj=ARG                                 #Compile with PROJ.x (ARG=yes or path)
#--with-liblzma=ARG                              #Include liblzma support (ARG=yes/no)
#--with-zstd=ARG                                 #Include zstd support (ARG=yes/no/installation_prefix)
#--with-pg=ARG                                   #Include PostgreSQL GDAL/OGR Support (ARG=yes,no)
#--with-grass=ARG                                #Include GRASS support (GRASS 5.7+, ARG=GRASS install tree dir)
#--with-libgrass=ARG                             #Include GRASS support based on libgrass (GRASS 5.0+)
#--with-cfitsio=ARG                              #Include FITS support (ARG=no or libcfitsio path)
#--with-pcraster=ARG                             #Include PCRaster (libcsf) support (ARG=internal, no or path)
#--with-png=ARG                                  #Include PNG support (ARG=internal, no or path)
#--with-dds=ARG                                  #Include DDS support (ARG=no, or path)
#--with-gta=ARG                                  #Include GTA support (ARG=no or libgta tree prefix)
#--with-pcidsk=ARG                               #Path to external PCIDSK SDK or internal (default)
#--with-libtiff=ARG                              #Libtiff library to use (ARG=internal, yes or path)
#--with-geotiff=ARG                              #Libgeotiff library to use (ARG=internal, yes or path)
#--with-jpeg=ARG                                 #Include JPEG support (ARG=internal, no or path)
#--with-charls                                   #Include JPEG-Lossless support
#--without-jpeg12                                #Disable JPEG 8/12bit TIFF support
#--with-gif=ARG                                  #Include GIF support (ARG=internal, no or path)
#--with-ogdi=ARG                                 #Include OGDI support (ARG=path)
#--with-fme=ARG                                  #Include FMEObjects support (ARG=FME_HOME path)
#--with-sosi=ARG                                 #Include SOSI support (ARG=SOSI lib Path, yes or no)
#--with-mongocxx=ARG                             #Include MongoCXX support (ARG=Path, yes or no)
#--with-boost-lib-path=ARG                       #Path to boost libraries for mongocxx client
#--with-mongocxxv3=ARG                           #Include MongoCXXv3 support (ARG=yes or no)
#--with-hdf4=ARG                                 #Include HDF4 support (ARG=path)
#--with-hdf5=ARG                                 #Include HDF5 support (ARG=path)
#--with-kea=ARG                                  #Include kealib (ARG=path to kea-config) [default=yes]
#--with-netcdf=ARG                               #Include netCDF support (ARG=no or netCDF tree prefix)
#--with-jasper=ARG                               #Include JPEG-2000 support via JasPer library (ARG=path)
#--with-openjpeg                                 #Include JPEG-2000 support via OpenJPEG 2.x library
#--with-fgdb=ARG                                 #Include ESRI File Geodatabase support (ARG=FGDP API Path, yes or no)
#--with-ecw=ARG                                  #Include ECW support (ARG=ECW SDK Path, yes or no)
#--with-kakadu=ARG                               #Include Kakadu/JPEG2000 support
#--with-mrsid=ARG                                #Include MrSID support (ARG=path to MrSID DSDK or no)
#--with-jp2mrsid=ARG                             #Enable MrSID JPEG2000 support (ARG=yes/no)
#--with-mrsid_lidar=ARG                          #Include MrSID/MG4 LiDAR support (ARG=path to LizardTech LiDAR SDK or no)
#--with-j2lura=ARG                               #Include JP2Lua support (ARG=no, lura SDK install path)
#--with-msg=ARG                                  #Enable MSG driver (ARG=yes or no)
#--with-oci=[ARG]                                #use Oracle OCI API from given Oracle home
#--with-oci-include=[DIR]
#--with-oci-lib=[DIR]                            #use Oracle OCI API libraries from given path
#--with-gnm                                      #Build GNM into shared library
#--with-mysql=ARG                                #Include MySQL (ARG=path to mysql_config) [default=no]
#--with-ingres=ARG                               #Include Ingres (ARG=$II_SYSTEM)
#--with-xerces=[ARG]                             #use Xerces C++ Parser from given prefix (ARG=path);
#--with-xerces-inc=[DIR]                         #path to Xerces C++ Parser headers
#--with-xerces-lib=[ARG]                         #link options for Xerces C++ Parser libraries
#--with-expat=[ARG]                              #use Expat XML Parser from given prefix (ARG=path);
#--with-expat-inc=[DIR]                          #path to Expat XML Parser headers
#--with-expat-lib=[ARG]                          #link options for Expat XML Parser libraries
#--with-libkml=[ARG]                             #use Google libkml from given prefix (ARG=path);
#--with-libkml-inc=[DIR]                         #path to Google libkml headers
#--with-libkml-lib=[ARG]                         #link options for Google libkml libraries
#--with-odbc=ARG                                 #Include ODBC support (ARG=no or path)
#--with-dods-root=ARG                            #Include DODS support (ARG=no or absolute path)
#--with-curl=ARG                                 #Include curl (ARG=path to curl-config.)
#--with-xml2=ARG                                 #Include libxml2 (ARG=path to xml2-config.)
#--with-spatialite=ARG                           #Include SpatiaLite support (ARG=no(default), yes, dlopen (only supported for Spatialite >= 4.1.2) or path)
#--with-spatialite-soname=ARG                    #Spatialite shared object name (e.g. libspatialite.so), only used if --with-spatiliate=dlopen
#--with-sqlite3=[ARG]                            #use SQLite 3 library [default=yes], optionally
#--with-rasterlite2=ARG                          #Include RasterLite2 support (ARG=no(default), yes or path)
#--with-pcre                                     #Include libpcre support (REGEXP support for SQLite)
#--with-teigha=path                              #Include Teigha DWG/DGN support
#--with-teigha-plt=platform                      #Teigha platform
#--with-idb=DIR                                  #Include Informix DataBlade support (DIR points to Informix root)
#--with-sde=DIR                                  #Include ESRI SDE support (DIR is SDE's install dir).
#--with-sde-version=VERSION                      #NUMBER  Set ESRI SDE version number (Default is 80).
#--with-epsilon=ARG                              #Include EPSILON support (ARG=no, yes or libepsilon install root path)
#--with-webp=ARG                                 #Include WEBP support (ARG=no, yes or libwebp install root path)
#--with-geos=ARG                                 #Include GEOS support (ARG=yes, no or geos-config
#--with-sfcgal=ARG                               #Include SFCGAL support (ARG=yes, no or sfcgal-config
#--with-qhull=ARG                                #Include QHull support (ARG=no, yes, internal)
#--with-opencl=ARG                               #Include OpenCL (GPU) support
#--with-opencl-include=ARG                       #OpenCL Include directory (with a CL subdirectory)
#--with-opencl-lib=ARG                           #OpenCL Link Flags (i.e. -L/xxx -lOpenCL)
#--with-freexl=ARG                               #Include freexl support (ARG=no, yes (default) or libfreexl install path)
#--with-libjson-c=ARG                            #Include libjson-c support (ARG=internal or libjson-c directory)
#--without-pam                                   #Disable PAM (.aux.xml) support
#--with-poppler=ARG                              #Include poppler(for PDF) support (ARG=no(default), yes or poppler install path)
#--with-podofo=ARG                               #Include podofo(for PDF) support (ARG=no(default), yes or podofo install path)
#--with-podofo-lib=ARG                           #podofo Link Flags (i.e. -L/xxx -lpodofo ...). Mainly for static libpodofo
#--with-podofo-extra-lib-for-test=ARG            #Additional libraries to pass the detection test, but not used for libgdal linking (i.e. -ljpeg ...). Mainly for static libpodofo
#--with-pdfium=ARG                               #Include pdfium (for PDF) support (ARG=no(default), yes or pdfium install path)
#--with-pdfium-lib=ARG                           #pdfium Link Flags (i.e. -L/xxx -lpdfium ...). Mainly for static libpdfium
#--with-pdfium-extra-lib-for-test=ARG            #Additional libraries to pass the detection test, but not used for libgdal linking (i.e. -ljpeg ...). Mainly for static libpdfium
#--with-gdal-ver=ARG                             #Override GDAL version
#--with-macosx-framework                         #Build and install GDAL as a Mac OS X Framework
#--with-perl                                     #Enable perl bindings
#--with-python=ARG                               #Enable python bindings (ARG=yes, no, or path to python binary)
#--with-java                                     #Include Java support (ARG=yes, no or JDK home path)  [default=no]
#--with-hdfs=ARG                                 #Include HDFS support (ARG=Path or no)
#--with-mdb                                      #Include MDB driver
#--with-jvm-lib=ARG                              #ARG is dlopen or points to Java libjvm path
#--with-jvm-lib-add-rpath                        #Add the libjvm path to the RPATH (no by default)
#--with-rasdaman=DIR                             #Include rasdaman support (DIR is rasdaman's install dir).
#--with-armadillo=ARG                            #Include Armadillo support for faster TPS transform computation (ARG=yes/no/path to armadillo install root) [default=no]
#--with-cryptopp=ARG                             #Include cryptopp support (ARG=yes, no or path)
#--with-crypto=ARG                               #Include crypto (from openssl) support (ARG=yes, no or path)
#--without-lerc                                  #Disable LERC library
#--with-null                                     #Enable NULL driver (only useful for development
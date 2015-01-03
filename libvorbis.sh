echo sed -i "" '''s/as_fn_error $$? "Ogg >= 1.0/echo "Ogg >= 1.0/g''' && \
sed -i '' 's/$srcdir\/configure/#/' autogen.sh && \
./autogen.sh && \
sed -i "" 's/as_fn_error $$? "Ogg >= 1.0/echo "Ogg >= 1.0/g' configure && \
sed -i "" "s/-O20/-O2/g" configure && \
sed -i "" "s/-O4/-O2/g" configure && \
sed -i '' 's/$ac_cv_func_oggpack_writealign/yes/' configure && \
emconfigure ./configure --prefix=`pwd`/build --build=x86_64 --disable-oggtest --disable-static --with-ogg=`pwd`/../libogg --with-ogg-includes=`pwd`/../libogg/install/include/ --with-ogg-libraries=`pwd`/../libogg/install/lib/ && \
EMCC_CFLAGS="--ignore-dynamic-linking" emmake make && \
emmake make install
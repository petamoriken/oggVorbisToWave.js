sed -i "" "s/-O20/-O2/g" configure && \
sed -i "" "s/-O4/-O2/g" configure && \
emconfigure ./configure --prefix=`pwd`/install --disable-static && \
emmake make && \
emmake make install
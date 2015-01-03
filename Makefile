OGG:=libogg-1.3.2
VORBIS:=libvorbis-1.3.4

OGG_URL:="http://downloads.xiph.org/releases/ogg/$(OGG).tar.gz"
VORBIS_URL:="http://downloads.xiph.org/releases/vorbis/$(VORBIS).tar.gz"

EMCC_OPTS:=-s EXPORTED_FUNCTIONS="['_sp_ov_start', '_sp_ov_end', '_sp_ov_to_wave']"
EMCC_OPTS+=-s LINKABLE=1
#EMCC_OPTS+=-O1 --memory-init-file 0 -s DISABLE_EXCEPTION_CATCHING=0 #--closure 1
#EMCC_OPTS+=--llvm-opts 3 --llvm-lto 0 

EMCC_LINKS:=-I libogg/install/include -Llibogg/install/lib -logg -I libvorbis/build/include -I libvorbis/include -Llibvorbis/build/lib -lvorbis


all: oggvorbis.js

oggvorbis.js: libogg libvorbis src/pre.js src/post.js src/head.js src/vorbis.c
	emcc $(EMCC_OPTS) --pre-js src/pre.js --post-js src/post.js $(EMCC_LINKS) src/vorbis.c libvorbis/lib/vorbisfile.c libvorbis/lib/.libs/libvorbis.a -o build.js && \
	cat src/head.js build.js > oggvorbis.js && \
	$(RM) -rf build.js
	
libogg: $(OGG).tar.gz
	tar xzvf $(OGG).tar.gz && \
	cd $(OGG) && \
	sed -i "" "s/-O20/-O2/g" configure && \
	sed -i "" "s/-O4/-O2/g" configure && \
	emconfigure ./configure --prefix=`pwd`/install --disable-static && \
	emmake make && \
	emmake make install && \
	cd ../ && mv $(OGG) libogg

libvorbis: $(VORBIS).tar.gz
	tar xzvf $(VORBIS).tar.gz && \
	cd $(VORBIS) && \
	echo sed -i "" '''s/as_fn_error $$? "Ogg >= 1.0/echo "Ogg >= 1.0/g''' && \
	../preautogen.sh && \
	sed -i "" 's/as_fn_error $$? "Ogg >= 1.0/echo "Ogg >= 1.0/g' configure && \
	sed -i "" "s/-O20/-O2/g" configure && \
	sed -i "" "s/-O4/-O2/g" configure && \
	sed -i '' 's/$ac_cv_func_oggpack_writealign/yes/' configure && \
	emconfigure ./configure --prefix=`pwd`/build --build=x86_64 --disable-oggtest --disable-static --with-ogg=`pwd`/../libogg --with-ogg-includes=`pwd`/../libogg/install/include/ --with-ogg-libraries=`pwd`/../libogg/install/lib/ && \
	EMCC_CFLAGS="--ignore-dynamic-linking" emmake make && \
	emmake make install && \
	cd ../ && mv $(VORBIS) libvorbis

$(OGG).tar.gz:
	test -e "$@" || wget $(OGG_URL)

$(VORBIS).tar.gz:
	test -e "$@" || wget $(VORBIS_URL)

clean:
	$(RM) -rf libogg libvorbis

distclean: clean
	$(RM) $(VORBIS).tar.gz $(OGG).tar.gz


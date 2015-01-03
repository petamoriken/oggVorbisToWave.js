// libogg and libvorbis function wrappers

window.Vorbis = {

	sp_ov_start:	Module.cwrap('sp_ov_start', 'number', []),
	sp_ov_end:		Module.cwrap('sp_ov_end', 'void', ['number']),
	sp_ov_to_wave:	Module.cwrap('sp_ov_to_wave', 'string', ['number'])

};

window.oggVorbisToWave = function(oggBuffer) {

	var ovFile, size, wavpc, ret, retBuffer, heap8, heapu32;

	heap8 = Module.HEAP8;
	heapu32 = Module.HEAPU32;

	FS.createDataFile("/", "data.ogg", new Uint8Array(oggBuffer), 1, void 0);

	ovFile = Vorbis.sp_ov_start();
	wavpc = Vorbis.sp_ov_to_wave(ovFile);

	FS.unlink("data.ogg");

	size = heapu32[wavpc + 1] +8;
	
	ret = new ArrayBuffer(size);
	retBuffer = new Uint8Array(ret);
	for(var i=0; i<size; ++i) {
		retBuffer[i] = heap8[wavpc + i];
	}

	Vorbis.sp_ov_end(ovFile);

	console.log(retBuffer);

	return ret;
};




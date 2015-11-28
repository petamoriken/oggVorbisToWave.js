var Vorbis = {

	sp_ov_to_wave:	cwrap('sp_ov_to_wave', 'number', ['number', "number", "number"]),
	sp_free_wave:	cwrap('sp_free_wave', 'void', ['number'])

};

var dataview = new DataView(buffer), isLittleEndian = (new Uint8Array((new Uint16Array([1])).buffer))[0];
var ENVIRONMENT_IS_REQUIRE = (typeof module !== "undefined" && module["exports"]);

function oggVorbisToWave(oggBuffer) {

	var oggpc, oggsize, wavpc, wavsize, ret;

	// set Ogg Buffer to memory
	oggsize = oggBuffer.byteLength;
	oggpc = _malloc(oggsize);
	writeArrayToMemory(new Uint8Array(oggBuffer), oggpc);

	// Ogg Buffer -> Wave Buffer
	wavpc = Vorbis.sp_ov_to_wave(oggpc, oggsize, isLittleEndian);

	_free(oggpc);

	wavsize = ( (isLittleEndian && wavpc % 4 === 0) ? HEAPU32[wavpc / 4 + 1] : dataview.getUint32(wavpc + 4, true) ) + 8;
	ret = buffer.slice(wavpc, wavpc + wavsize);

	Vorbis.sp_free_wave(wavpc);

	return ret;

}

// export
if(ENVIRONMENT_IS_REQUIRE) {
	module["exports"] = oggVorbisToWave;
} else if(ENVIRONMENT_IS_WEB) {
	window["oggVorbisToWave"] = oggVorbisToWave;
} else if(ENVIRONMENT_IS_WORKER) {
	onmessage = function(e) {
		postMessage(oggVorbisToWave(e.data));
	}
}

})();
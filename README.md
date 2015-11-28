OggVorbis To Wave
==================

##What this is

This library makes OggVorbis `ArrayBuffer` -> Wave `ArrayBuffer`.  
Enable to play OggVorbis in Frontend-JavaScript by making Web Audio API's `AudioBuffer` in Safari, MS Edge and IE10~11 (use [WAAPISim]).

このライブラリは OggVorbis の `ArrayBuffer` を Wave の `ArrayBuffer` に変換することが出来ます。  
すなわち Web Audio API を使って Safari や MS Edge, IE10~11 でも OggVorbis から `AudioBuffer` を作ることができるようになります。ただし IE10~11 では Web Audio API 自体サポートされていませんので、[WAAPISim] を使うことになります。

ブラウザの script タグから読み込んだ場合は `window.oggVorbisToWave` を、[Browserify] においては `require("./oggVorbisToWave")` で使用できます。また WebWorker の `new Worker("oggVorbisToWave.js")` で使えます。

このライブラリは [Emscripten] を利用して作られています。


##How to use

	var ctx = new AudioContext();
	var req = new XMLHttpRequest();

	req.open("GET", "test.ogg", true);
	req.responseType = "arraybuffer";
	req.onload = function() {

		var oggArrayBuffer = req.response;
		ctx.decodeAudioData(oggArrayBuffer, doSomething, function() {
			// ctx.decodeAudioData doesn't support OggVorbis (Safari, MS Edge, IE10~11)
			var waveArrayBuffer = oggVorbisToWave(oggArrayBuffer);
			ctx.decodeAudioData(waveArrayBuffer, doSomething);
		});
		
	};
	req.send();

	function doSomething(AudioBuffer) {
		// do something
	}

in WebWorker:

	var worker = new Worker("oggVorbisToWave.js");

	worker.postMessage(oggArrayBuffer, [oggArrayBuffer]);
	worker.onmessage = function(e) {
		ctx.decodeAudioData(e.data, doSomething);
	}


##License

Under MIT License

this software includes the work that is distributed in the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0)  
[libvorbis.js] - port of libvorbis to JavaScript using emscripten  
by Bjorn Melinder bjorn@soundtrap.com


[WAAPISim]:https://github.com/g200kg/WAAPISim
[Browserify]:http://browserify.org/
[Emscripten]:https://github.com/kripken/emscripten
[libvorbis.js]:https://github.com/bjornm/libvorbis-js
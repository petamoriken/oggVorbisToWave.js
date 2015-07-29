OggVorbis To Wave
==================

##What this is

Enable to play OggVorbis in Frontend-JavaScript by making Web Audio API's `AudioBuffer` in Safari and IE10~11 (use [WAAPISim]).

Web Audio API を使って Safari や IE10~11 でも OggVorbis から `AudioBuffer` を作ることができます。ただし IE10~11 では Web Audio API 自体サポートされていませんので、[WAAPISim] を使うことになります。

このライブラリはブラウザの script タグから読み込んだ場合は `window.oggVorbisToWave` を、[Browserify] や Node.js などの CommonJS においては `require("./oggVorbisToWave")` で使用できます。  
使用することで OggVorbis の `ArrayBuffer` を Wave の `ArrayBuffer` に変換することが可能です。

このライブラリは [Emscripten] を使用しています。


##How to use

	var ctx;
	try {
		ctx = new (window.AudioContext || window.webkitAudioContext)();
	} catch(e) {
		ctx = null;
		console.log("Can't use Web Audio API");
	}

	if(ctx !== null) {
		var req = new XMLHttpRequest();
		req.open("GET", "test.ogg", true);
		req.responseType = "arraybuffer";
		req.onload = function(){
		
			var oggArrayBuffer = req.response;
			ctx.decodeAudioData(oggArrayBuffer, doSomething, function() {

				// ctx.decodeAudioData doesn't support OggVorbis (Safari, IE10~11)
				var waveArrayBuffer = oggVorbisToWave(oggArrayBuffer);
				ctx.decodeAudioData(waveArrayBuffer, doSomething);
				
			});
			
		};
		req.send();
	}
	
	function doSomething(AudioBuffer) {
		// do something
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
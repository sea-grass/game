function streamFactory(opts) {
    var buf = new Array(bufferSize);
    var bufPos = 0;
    var lineEnd = '\n'.charCodeAt(0);
    var bufferSize = opts.bufferSize;
    var print = opts.print;
    return function stream(asciiCode) {
        buf[bufPos] = asciiCode;
        if (asciiCode == lineEnd) {
            console.log(buf.map(x => String.fromCharCode(x)).join(''));
            print(buf.map(x => String.fromCharCode(x)).join(''));
            bufPos = 0;
        } else {
            bufPos++;
        }
    }
}

function preRun() {
    function stdin() {
        // return ascii code of character, or null if no input
        console.log('stdin was called');

        return prompt('Input') || null;
    }
    var stdout = streamFactory({
        bufferSize: 1000,
        print: (str) => console.log("stdout", str)
    });
    var stderr = streamFactory({
        bufferSize: 1000,
        print: (str) => console.log("stderr", str)
    });
    FS.init(stdin, stdout, stderr);
}

Module['preRun'].push(preRun);
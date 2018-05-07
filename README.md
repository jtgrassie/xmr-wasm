# A Monero WebAssembly based miner

The core aim of this project is to provide a completely open source, browser based, Monero miner. It has a companion project, [my fork](https://github.com/jtgrassie/xmr-node-proxy) of Snipa's **xmr-node-proxy** which adds a WebSocket based branch for allowing this miner to connect through to various mining pools.

Whilst browser mining can be used maliciously, there are many non-malicious use-cases, such as charitable donations (the very reason this project started). A defining aspect to whether web mining is done in a fair way is that of user consent. My recommendation is therefore to ensure the end-user knows the page will be mining and offer them controls such as starting and stopping, and options for how much processing time the mining should use.


## Compiling

To compile, you first need a working setup of the WebAssembly toolchain. Follow instructions at [webassembly.org](http://webassembly.org/getting-started/developers-guide/).

Then simply run `make` in the root directory of this repository. This will output to the build/debug folder.

For an optimized release version, just run `make TYPE=release`.

To test locally, just tack `run` onto the end of the above commands, which will start a local server for testing (required to load the WebAssembly module).

## License

Please see [LICENSE](LICENSE)



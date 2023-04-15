# rankserver-cpp

Pure C++ implementation of my Python-based [rank server](https://github.com/goromal/anixpkgs/tree/bf7a4a905ab5cd29aead770a4a9d04a7bb59a17f/pkgs/python-packages/flasks/rankserver) with flask. Requires a [patched version of Crow](https://github.com/goromal/Crow) that allows for runtime configuration of the server's static data directory.

```
Options:
  -h [ --help ]         print usage
  -p [ --port ] arg     port to serve on (default: 4000)
  -d [ --data-dir ] arg data directory to process (default: ./data)
```

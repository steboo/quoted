# quoted

quoted is an implementation of the Quote of the Day Protocol which is an Internet standard specified in [RFC 865](https://tools.ietf.org/html/rfc865) by J. Postel. A quote of the day service ignores all input, and simply responds with a quote, making this a potentially useful debugging or measurement tool.

It is also a straightforward example of writing TCP and UDP servers in C with POSIX (Berkeley) sockets.

# Usage

By default, quoted listens on port 17 with TCP:

```
$ ./quoted
```

To listen on UDP instead, use the `-u` option:

```
$ ./quoted -u
```

To change the default port, use the `-p` option:

```
$ ./quoted -p 8888
```

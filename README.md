ctrGL - a far-from-complete OpenGL implementation for the POS PICA
=====

Heavily based on code from [ctrulib](https://github.com/smealum/ctrulib).

This project would never have been possible without all the great people behind 3Dbrew and ctrulib.
Special thanks to Smealum for blessing us with code exec in the first place.

B-but muh documentation
----
RTFH.

How does this work?
----
RTFS.

Note
----
*ctrGL* assumes that you know what you're doing (which no one really does at this point).
If you give it invalid inputs (including incorrect enums), it will happily pass along your bullshit values to the POS PICA, which will happily take care of teaching you a thing or two about pain and suffering.

*ctrGL* does barely any input verification and sanity checks, because
- it would be a waste of precious CPU cycles
- nobody checks for GL errors anyway and a crash is easier to debug than something not happening when it was supposed to

So yeah, if you want babby's first GL library, go play with nVidia drivers. (or render in software you ape)

# bond_core

A bond allows two processes, A and B, to know when the other has terminated, either cleanly or by crashing. 
The bond remains connected until it is either broken explicitly or until a heartbeat times out.

## License 

The implementation of bond itself is under: BSD-3-Clause
bond_core includes a vendored version of [smclib](https://smc.sourceforge.net) which is under: MPL-1.1


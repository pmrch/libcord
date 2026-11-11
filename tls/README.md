# libcord
A zero-dependency, zero-heap Discord library written in pure C.

## Current Status
- [x] X25519 (Radix-51, register-optimized, differential fuzzed against curve25519-donna)
- [x] Custom DNS Resolver
- [x] TLS 1.3 (Handshake up to ServerHello)
- [x] HMAC-SHA256
- [ ] ChaCha20-Poly1305
- [ ] WebSocket (WSS)
- [ ] Discord Gateway

## Why?
Because depending on having installed OpenSSL dev headers, or any alternatives that may break on any
of the supported platforms is better avoided. 

## Build
(Instructions pending)

## Warning
I don't take any responsibility for anything. It's public. Do what you want.
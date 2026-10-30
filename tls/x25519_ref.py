# X25519 — readable reference implementation (NOT constant-time)
P = 2**255 - 19
A24 = 121665  # (A - 2) / 4 where A = 486662

def decode_scalar(k: bytes) -> int:
    # RFC 7748: clamp
    k = bytearray(k)
    k[0] &= 248
    k[31] &= 127
    k[31] |= 64
    return int.from_bytes(k, "little")

def decode_u(u: bytes) -> int:
    # RFC 7748: mask the top bit
    u = bytearray(u)
    u[31] &= 127
    return int.from_bytes(u, "little") % P

def encode_u(u: int) -> bytes:
    return (u % P).to_bytes(32, "little")

def cswap(swap: int, a: int, b: int):
    # swap if swap == 1
    dummy = swap * ((a - b) % P)
    return (a - dummy) % P, (b + dummy) % P

def x25519(k: bytes, u: bytes) -> bytes:
    k_int = decode_scalar(k)
    x1 = decode_u(u)
    x2, z2 = 1, 0
    x3, z3 = x1, 1
    swap = 0
    for t in range(254, -1, -1):          # bits 254..0
        kt = (k_int >> t) & 1
        swap ^= kt
        x2, x3 = cswap(swap, x2, x3)
        z2, z3 = cswap(swap, z2, z3)
        swap = kt
        # Montgomery ladder step
        A  = (x2 + z2) % P
        AA = (A * A) % P
        B  = (x2 - z2) % P
        BB = (B * B) % P
        E  = (AA - BB) % P
        C  = (x3 + z3) % P
        D  = (x3 - z3) % P
        DA = (D * A) % P
        CB = (C * B) % P
        x3 = (DA + CB) % P
        x3 = (x3 * x3) % P
        z3 = (DA - CB) % P
        z3 = (z3 * z3) % P
        z3 = (z3 * x1) % P
        x2 = (AA * BB) % P
        z2 = (E * (AA + A24 * E)) % P
    x2, x3 = cswap(swap, x2, x3)
    z2, z3 = cswap(swap, z2, z3)
    return encode_u((x2 * pow(z2, P - 2, P)) % P)
  
k = bytes.fromhex("a546e36bf0527c9d3b16154b82465edd62144c0ac1fc5a18506a2244ba449ac4")
u = bytes.fromhex("e6db6867583030db3594c1a424b15f7c726624ec26b3353b10a903a6d0ab1c4c")
out = x25519(k, u)
assert out.hex() == "c3da55379de9c6908e94ea4df28d084f32eccf03491c71f754b4075577a28552"
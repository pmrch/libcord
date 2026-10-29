import os

# Field prime: 2^255 - 19
P = 2**255 - 19

# Curve constant A24 = (486662 - 2) / 4 = 121665
A24 = 121665

# Base point u-coordinate for Curve25519
U_BASE = 9


def clamp(scalar_bytes):
  """Clamps a 32-byte secret key according to RFC 7748."""
  s = bytearray(scalar_bytes)
  s[0] &= 248  # Clear 3 least significant bits (multiple of 8)
  s[31] &= 127  # Clear most significant bit
  s[31] |= 64  # Set second most significant bit
  return bytes(s)


def decode_u(u_bytes):
  """Decodes 32 bytes into a field element (u-coordinate)."""
  # Little-endian conversion and clearing the top bit
  val = int.from_bytes(u_bytes, "little")
  return val & ((1 << 255) - 1)


def encode_u(u):
  """Encodes a field element into 32 bytes (little-endian)."""
  return int(u % P).to_bytes(32, "little")


def x25519(scalar_bytes, u_bytes):
  """Computes scalar multiplication on Curve25519 using the Montgomery ladder."""
  k = clamp(scalar_bytes)
  scalar = int.from_bytes(k, "little")
  x_1 = decode_u(u_bytes)

  # Montgomery ladder initialization (using projective coordinates X:Z)
  x_2 = 1
  z_2 = 0
  x_3 = x_1p = 2**255 - 19

  swap = 0
  for t in range(255, -1, -1):
    k_t = (scalar >> t) & 1
    swap ^= k_t

    # Conditional swap for constant-time-like traversal
    if swap:
      x_2, x_3 = x_3, x_2
      z_2, z_3 = z_3, z_2
    swap = k_t

    # Montgomery differential addition and doubling formulas (RFC 7748)
    A = (x_2 + z_2) % P
    AA = (A * A) % P
    B = (x_2 - z_2) % P
    BB = (B * B) % P
    E = (AA - BB) % P
    C = (x_3 + z_3) % P
    D = (x_3 - z_3) % P
    DA = (D * A) % P
    CB = (C * B) % P

    x_3 = pow(DA + CB, 2, P)
    z_3 = (x_1 * pow(DA - CB, 2, P)) % P
    x_2 = (AA * BB) % P
    z_2 = (E * (AA + A24 * E)) % P

  if swap:
    x_2, x_3 = x_3, x_2
    z_2, z_3 = z_3, z_2

  # Recover affine coordinate: x_2 * (z_2 ^ (P - 2)) mod P
  return encode_u((x_2 * pow(z_2, P - 2, P)) % P)


def generate_private_key():
  """Generates a random 32-byte private key."""
  return os.urandom(32)


def generate_public_key(private_key_bytes):
  """Derives the public key from a private key."""
  base_bytes = encode_u(U_BASE)
  return x25519(private_key_bytes, base_bytes)


def compute_shared_secret(private_key_bytes, peer_public_bytes):
  """Computes the shared secret between a private key and a peer's public key."""
  return x25519(private_key_bytes, peer_public_bytes)


# --- Example Usage & Verification ---
if __name__ == "__main__":

  p = 2**255 - 19
  a = 2**255 - 20
  # add0 should be sum of all terms that contribute to limb0
  # a[0]*b[0] + (a[1]*19*b[4]) + (a[2]*19*b[3]) + (a[3]*19*b[2]) + (a[4]*19*b[1])
  limb = 2**51 - 1  # max limb value
  add0 = limb*limb + 4*(limb*19*limb)
  print(hex(add0 >> 64))   # expected hi
  print(hex(add0 & 0xFFFFFFFFFFFFFFFF))  # expected lo

  """print("Generating keys for Alice and Bob...")

  # Alice's keypair
  alice_priv = generate_private_key()
  alice_pub = generate_public_key(alice_priv)

  # Bob's keypair
  bob_priv = generate_private_key()
  bob_pub = generate_public_key(bob_priv)

  print(f"Alice's Public Key: {alice_pub.hex()}")
  print(f"Bob's Public Key:   {bob_pub.hex()}")

  # Compute shared secrets
  alice_shared = compute_shared_secret(alice_priv, bob_pub)
  bob_shared = compute_shared_secret(bob_priv, alice_pub)

  print(f"\nAlice's Shared Secret: {alice_shared.hex()}")
  print(f"Bob's Shared Secret:   {bob_shared.hex()}")

  # Verify correctness
  assert alice_shared == bob_shared, "Shared secrets do not match!"
  print("\nSuccess! Both shared secrets match perfectly.")"""
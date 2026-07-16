import struct

def encode(s):
    exponent_bits = 0b11111111111

    message_bytes = s.encode("utf-8")
    message_number = int.from_bytes(message_bytes, byteorder="big")

    exponent = exponent_bits << 52

    answer = message_number | exponent

    raw = struct.pack(">Q", answer)
    x = struct.unpack(">d", raw)[0]

    return x

def decode(input):
    raw_bytes = struct.pack(">d", input)
    bits_as_integer = struct.unpack(">Q", raw_bytes)[0]

    mask = (1 << 48) - 1 # useful technique to know, gets me all 1s (1 << x) - 1

    extract = bits_as_integer & mask

    message_bytes = extract.to_bytes(6, byteorder="big")
    message_bytes = message_bytes.lstrip(b"\x00")
    message = message_bytes.decode("utf-8")

    return message


x = encode("hello")
print(x)
print(decode(x))

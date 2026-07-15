with open("computer_systems/1.uint64", "rb") as file:
    data = file.read()

print(data)

number = int.from_bytes(data)

print(number)


def encode(value):
    ba = bytearray()
    mask = 127

    if value == 0:
        return bytes(1)

    
    while(value):
        payload = value & mask
        value = value >> 7 

        if value != 0:
            completed_byte = payload | 128
            ba.append(completed_byte)
        else:
            ba.append(payload)
        
    return bytes(ba)
        
# Follow these steps for each byte.
# Start with an empty result and bit position:
# Read the next byte.
# Extract its lowest 7 payload bits.
# Move that payload into its correct position.
# The first payload moves left by 0:
# 0010110 << 0
# The second payload moves left by 7:
# 0000001 << 7
# A third would move left by 14, then 21, and so on.
# Combine the positioned payload with result.
# Inspect the byte’s highest bit. Is byte & continuation_mask zero?

def decode(encoded):
    # varint bytes -> uint64 integer
    pass


for value in [0, 1, 127, 128, 150, 16383, 16384]:
    encoded = encode(value)
    print(value, encoded.hex(), [format(b, "08b") for b in encoded])
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
        

def decode(encoded):
    # varint bytes -> uint64 integer
    pass


for value in [0, 1, 127, 128, 150, 16383, 16384]:
    encoded = encode(value)
    print(value, encoded.hex(), [format(b, "08b") for b in encoded])
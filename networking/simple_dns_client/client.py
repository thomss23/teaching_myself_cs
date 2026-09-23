import struct
import socket
import secrets

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.settimeout(2)

transaction_id = secrets.randbits(16)

header = struct.pack(
    "!HHHHHH",
    transaction_id,
    0x0100,  # flags
    1,       # one question
    0,       # answers
    0,       # authority records
    0,       # additional records
)

domain_name = "wikipedia.com"

labels = domain_name.split(".")

qname = b""
for label in labels:
    label_bytes = label.encode("ascii")
    qname += bytes([len(label_bytes)])
    qname += label_bytes

qname += b"\x00"
qtype = struct.pack("!H", 1)   # A record
qclass = struct.pack("!H", 1)  # Internet

request_bytes = header + qname + qtype + qclass

sock.sendto(request_bytes, ("1.1.1.1", 53))

response_bytes, server_adress = sock.recvfrom(4096)

sock.close()

# print(response_bytes.hex(" "))
# print(repr(response_bytes))

# read header 12 bytes
# verify transaction ID
# check response code
# skip question
# for each answer:
#     skip NAME
#     read TYPE, CLASS, TTL, RDLENGTH
#     read RDATA
#     print RDATA if TYPE=A and CLASS=IN

offset = 12 # i am skipping 12 bytes of the header

def skip_name(data, offset):
    while True:
        length = data[offset]

        if length == 0:
            return offset + 1

        # Compressed name: two-byte pointer.
        if (length & 0xC0) == 0xC0:
            return offset + 2

        offset += 1 + length


response_id, flags, question_count, answer_count, _, _ = struct.unpack(
    "!HHHHHH",
    response_bytes[:12],
)

if response_id != transaction_id:
    raise ValueError("transaction ID does not match")

response_code = flags & 0x000F

if response_code != 0:
    raise ValueError(f"DNS error code: {response_code}")

offset = 12

for _ in range(question_count):
    offset = skip_name(response_bytes, offset)
    offset += 4  # QTYPE: 2 bytes, QCLASS: 2 bytes

for _ in range(answer_count):
    offset = skip_name(response_bytes, offset)

    record_type, record_class, ttl, data_length = struct.unpack_from(
        "!HHIH",
        response_bytes,
        offset,
    )

    offset += 10  # TYPE 2 + CLASS 2 + TTL 4 + RDLENGTH 2

    record_data = response_bytes[offset:offset + data_length]
    offset += data_length

    if record_type == 1 and record_class == 1 and data_length == 4:
        ip_address = socket.inet_ntoa(record_data)
        print(ip_address)

    


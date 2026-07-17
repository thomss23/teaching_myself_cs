def truncate():
    with open("computer_systems/uc-exercise/cases", "rb") as file:
        ans = []
        for line in file:
            line = line.removesuffix(b"\n")
            limit_byte = line[0]
            text = line[1:]

            if len(text) <= limit_byte:
                ans.append(text.decode("utf-8"))
                continue


            while(True):
                truncated_from_bytes = text[0:limit_byte]
                try:
                    truncated = truncated_from_bytes.decode("utf-8")
                    ans.append(truncated)
                    break
                except UnicodeDecodeError:
                    limit_byte = limit_byte - 1
        
        return ans

print(truncate())
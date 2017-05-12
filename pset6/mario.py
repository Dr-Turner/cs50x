print("Height: ", end='')

while True:
    height = input()

    # check if int
    try:
        height = int(height)
    except ValueError:
        print("Retry: ", end='')
        continue

    # check if suitable value
    if height >= 0 and height <= 23:
        break
    else:
        print("Height: ", end='')

# draw pyramid
for i in range(height):
    hashes = "#" * (i + 1)
    line = hashes.rjust(height) + '  ' + hashes
    print(line)
import sys


def shift(c, k):
    """shifts  and returns char c by offset determined by char k"""
    c_upper = c.isupper()
    k_upper = k.isupper()
    # find numeric value of c (offset so a == 0).
    c = ord(c) - 65 if c_upper else ord(c) - 97
    k = ord(k) - 65 if k_upper else ord(k) - 97

    # shift according to (k)ey char and wrap
    c += k
    c %= 26

    # return c converted to string and unoffset
    return chr(c + 65) if c_upper else chr(c + 97)


# check only one keyword supplied:
if len(sys.argv) != 2:
    print("ERROR! One keyword should be supplied")
    sys.exit()

key = sys.argv[1]

# check key only contains alphabetical characters
for letter in key:
    if not letter.isalpha():
        print("ERROR! KEY must be a word containing only letters")

plain = input("plaintext: ")

# find number of letters in plaintext. and ensure key is duplicated enough times.
alpha_chars = sum([1 for letter in plain if letter.isalpha()])

while len(key) < alpha_chars:
    key += key

cipher = ''
# keep count of key string index with i
i = 0

for letter in plain:
    if not letter.isalpha():
        # skip over any non-letters.
        cipher += letter
    else:
        cipher += shift(letter, key[i])
        i += 1

print("ciphertext: %s" % cipher)

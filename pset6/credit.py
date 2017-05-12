"""
    product() return total of the product of the number passed in. eg: 14 = 1 + 4 = 5
"""
def product(n):
    total = 0

    for i in str(n):
        total += int(i)

    return total


"""
    checksum() checks a card number and determines if it is authentic or not. returns boolean
"""
def checksum(n):
    total = 0
    # reverse card number for better readability
    n = n[::-1]

    # grab and double every odd digit
    odd_digits = [int(i) * 2 for i in n[1::2]]
    # grab every even digit
    even_digits = [int(i) for i in n[::2]]

    # add the products of the odd indexed numbers together
    for digit in odd_digits:
        if digit >= 10:
            total += product(digit)
        else:
            total += digit

    # add sum of even digits
    total += sum(even_digits)

    # determine if valid
    if total % 10 == 0:
        return True
    else:
        return False


while True:
    cardno = input("Number: ")
    # check card number is a valid integer (without changing it to one)
    try:
        int(cardno)
    except  ValueError:
        print("Card number must be an integer!")
        continue
    else:
        break

# get length of number
l = len(cardno)

card = "INVALID"

# check card attributes and assign card type string if checksum passes 
if (l == 13 or l == 16) and cardno[0] == '4':
    if checksum(cardno):
        card = "VISA"
elif l == 15 and cardno[:2] in ['34', '37']:
    if checksum(cardno):
        card = "AMEX"
elif l == 16 and cardno[:2] in ['51', '52', '53', '54', '55']:
    if checksum(cardno):
        card = "MASTERCARD"

print(card)
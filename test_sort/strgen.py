import random
import string
print("number of generate string:")
end = int(input())
print("output file name:")
file_name = input()
with open(file_name,'w') as f: 
    for i in range(end):
        f.write(''.join(random.SystemRandom().choice(string.ascii_lowercase) for _ in range(random.randint(5,10))) + '\n')
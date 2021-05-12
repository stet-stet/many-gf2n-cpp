# the flags are obtained with:
# g++ -Q --help=optimizers -O0 > gppO0flags.txt
# g++ -Q --help=optimizers -O1 > gppO1flags.txt
with open("gppO0flags.txt",'r') as file:
  one = file.readlines()
with open("gppO1flags.txt",'r') as file:
  two = file.readlines()
for x,y in zip(one,two):
  if x!=y: print(x.strip().split()[0].strip())

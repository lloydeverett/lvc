
# Quick python script to take a list of comma-separated identifiers
# and turn them into a list of comma-separated string literals in CPP.
# Takes input from stdin. Give it an empty line when the input is finished.

# Quite hacky. Need to actually learn python properly.

thisline = raw_input()
x = thisline
while thisline != "":
    thisline = raw_input()
    x += thisline

x = x.replace(" ", "")
if x.endswith(","):
    x = x[:-1]

ar = x.split(",")
for elem in ar:
    print ("\"" + elem + "\",")

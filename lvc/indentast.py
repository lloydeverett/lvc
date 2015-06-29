
# Takes dump of AST and makes it more legible.
# Ideally we wouldn't have to have a ptyhon script do this but I didn't want
# to add complexity to the AST classes.

thisline = raw_input()
x = thisline
while thisline != "":
    thisline = raw_input()
    x += thisline
x = x.replace(" ", "")

class Parser:
    str = ""
    pos = 0
    __init

pos = 0

def strOfSpaces(count):
    str = ""
    for i in xrange(0, count):
        str += " "
    return str

def printBasic(indent)
    global x
    print strOfSpaces(indent) +

print output

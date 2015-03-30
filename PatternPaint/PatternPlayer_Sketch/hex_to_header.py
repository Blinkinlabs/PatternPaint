#!/usr/bin/python
import argparse

def ParseLine(line):
  """ Parse a line of an Intel HEX file into its component parts """
  if line[0] != ':':
    raise "Bad line beginning"

  line = line.rstrip() # There is a trailing crlf or something?

  byteCount = int(line[1:3], 16)
  address =   int(line[3:7],16)
  type =      int(line[7:9],16)
  data =      line[9:-2]
  checksum =  int(line[-2:],16)

#  for c in data:
#    print "> ", c, "<"

  return type, address, byteCount, data, checksum


def HexToHeader(fileName, animationName):
  """ Super simple hex-to-c++-header converter. Has a single block of contiguous memory """

  headerData = ''
  with open(fileName) as fp:
    for line in fp:
      type, address, byteCount, data, checksum = ParseLine(line)
      
      if(type == 0):
        if(2*address == len(headerData)):  #Note there are 2 chars per address
          headerData += ''.join(data)
        else:
          print "This hex file appears to have non-contiguous data! Please upgrade this script"
          exit(1)
      elif(type == 1):
        # type one is stop, so just stop.
        break;
      else:
        print "type not understood: ", type
        exit(1)

  print "#ifndef %s_SKETCH_H"%(animationName)
  print "#define %s_SKETCH_H"%(animationName)
  print ""
  print "//Automatically generated file..."
  print ""
  print "#include <stdint.h>"
  print ""
  print "#define %s_ADDRESS  0x%04X"%(animationName, 0)
  print "#define %s_LENGTH   %i"%(animationName, len(headerData)/2)
  print ""

  print "const uint8_t %s_DATA[] = {"%(animationName)

  for i in range(0, len(headerData)/2):
    print "0x%02X,"%(int(headerData[i*2:i*2+2],16)),
    if(i%(16) == 15):
      print ''

  print "};"
  print ""
  print "#endif // %s_SKETCH_H"%(animationName)

parser = argparse.ArgumentParser('Convert an intel hex file into a c++ header')
parser.add_argument('i', help = 'location of the hex file to read')
parser.add_argument('n', help = 'sketch name')
args = parser.parse_args()

HexToHeader(args.i, args.n)

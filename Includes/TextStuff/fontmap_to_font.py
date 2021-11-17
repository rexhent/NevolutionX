#!/usr/bin/env python3

import sys
import re
import json

def main():
    if len(sys.argv) != 3:
        print(f"usage: {sys.argv[0]} <infile> <outfile>")
        exit(1)

    with open(sys.argv[1], 'r') as fontmap:
        print(fontmap.readline()[:-1])
        print(fontmap.readline()[:-1])
        print(fontmap.readline()[:-1])
        print(fontmap.readline()[:-1])

        characters = {}
        while True:
            fontmap_line = fontmap.readline()[:-1]
            if not fontmap_line or fontmap_line[0] == 'k':
                break
            
            linelist = list(filter(None, re.split('\=|\ ', fontmap_line)[1:]))

            dictionary = {}
            for i in range(2, len(linelist), 2):
                dictionary[linelist[i]] = linelist[i+1]
            characters[linelist[1]] = dict(dictionary)
            characters[linelist[1]].pop('page')
            characters[linelist[1]].pop('chnl')

        with open(sys.argv[2], 'w') as outfile:
            outfile.write('void renderChar(int x, int y, int w, int h, int xoffset,')
            outfile.write(' int yoffset,\n                int xadvance, std::pair<int,int> &coords)\n{\n')
            outfile.write('\n')
            outfile.write('}\n\n')
            outfile.write('std::pair<int,int> renderString(std::string s, std::pair<int,int> &coords)\n')
            outfile.write('{\n  for (auto c: s) {\n')
            outfile.write('    switch (c[i]) {\n')
            for a in characters.keys():
                currItem = characters[a]
                outfile.write(f'     case {a}:\n')
                outfile.write(f'      renderChar({str(currItem["x"])}, {str(currItem["y"])}, ')
                outfile.write(f'{currItem["width"]}, {currItem["height"]}, ')
                outfile.write(f'{currItem["xoffset"]}, {currItem["yoffset"]}, {currItem["xadvance"]}, coords);\n')
                outfile.write('      break;\n')

            outfile.write('    default:\n      self(\'?\');\n      break;\n    }\n  }\n}')

main()

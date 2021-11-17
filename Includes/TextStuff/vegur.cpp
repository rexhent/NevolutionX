void Renderer::renderChar(int x, int y, int w, int h, int xoffset, int yoffset,
                          int xadvance, std::pair<int,int> &coords)
{
  std::get<1>(coords) += xadvance;
}

void Renderer::renderString(std::string s, std::pair<int,int> coords)
{
  for (auto c: s) {
    switch (c[i]) {
     case 32:
      renderChar(506, 104, 3, 1, -1, 71, 14, coords);
      break;
     case 33:
      renderChar(316, 150, 9, 43, 3, 16, 15, coords);
      break;
     case 34:
      renderChar(475, 309, 14, 16, 3, 14, 19, coords);
      break;
     case 35:
      renderChar(188, 153, 26, 43, 2, 17, 29, coords);
      break;
     case 36:
      renderChar(148, 107, 23, 46, 3, 15, 29, coords);
      break;
     case 37:
      renderChar(310, 106, 42, 43, 3, 17, 48, coords);
      break;
     case 38:
      renderChar(435, 105, 35, 43, 1, 16, 38, coords);
      break;
     case 39:
      renderChar(502, 308, 6, 16, 3, 14, 11, coords);
      break;
     case 40:
      renderChar(411, 0, 12, 53, 4, 12, 17, coords);
      break;
     case 41:
      renderChar(369, 0, 13, 53, 1, 12, 17, coords);
      break;
     case 42:
      renderChar(381, 312, 25, 23, 2, 18, 28, coords);
      break;
     case 43:
      renderChar(111, 335, 28, 30, 2, 27, 33, coords);
      break;
     case 44:
      renderChar(490, 309, 11, 16, -2, 51, 13, coords);
      break;
     case 45:
      renderChar(6, 56, 15, 4, 3, 40, 21, coords);
      break;
     case 46:
      renderChar(97, 366, 8, 8, 2, 51, 13, coords);
      break;
     case 47:
      renderChar(379, 54, 25, 51, -1, 13, 24, coords);
      break;
     case 48:
      renderChar(194, 197, 27, 42, 1, 17, 29, coords);
      break;
     case 49:
      renderChar(79, 294, 17, 40, 5, 18, 29, coords);
      break;
     case 50:
      renderChar(151, 241, 25, 41, 2, 17, 29, coords);
      break;
     case 51:
      renderChar(379, 193, 24, 42, 2, 17, 29, coords);
      break;
     case 52:
      renderChar(0, 294, 27, 40, 2, 18, 29, coords);
      break;
     case 53:
      renderChar(355, 236, 24, 41, 3, 18, 29, coords);
      break;
     case 54:
      renderChar(249, 196, 26, 42, 2, 17, 29, coords);
      break;
     case 55:
      renderChar(28, 294, 26, 40, 2, 18, 29, coords);
      break;
     case 56:
      renderChar(328, 194, 25, 42, 2, 17, 29, coords);
      break;
     case 57:
      renderChar(302, 195, 25, 42, 2, 17, 29, coords);
      break;
     case 58:
      renderChar(500, 276, 8, 31, 2, 28, 13, coords);
      break;
     case 59:
      renderChar(296, 280, 12, 39, -2, 28, 13, coords);
      break;
     case 60:
      renderChar(140, 328, 26, 30, 3, 28, 32, coords);
      break;
     case 61:
      renderChar(0, 367, 29, 14, 3, 35, 35, coords);
      break;
     case 62:
      renderChar(219, 323, 25, 30, 3, 28, 32, coords);
      break;
     case 63:
      renderChar(292, 151, 23, 43, 1, 16, 25, coords);
      break;
     case 64:
      renderChar(71, 108, 45, 46, 3, 21, 51, coords);
      break;
     case 65:
      renderChar(433, 150, 36, 42, 0, 16, 36, coords);
      break;
     case 66:
      renderChar(242, 152, 25, 43, 4, 16, 31, coords);
      break;
     case 67:
      renderChar(0, 165, 34, 43, 2, 16, 37, coords);
      break;
     case 68:
      renderChar(471, 104, 34, 43, 4, 16, 40, coords);
      break;
     case 69:
      renderChar(280, 238, 24, 41, 4, 17, 29, coords);
      break;
     case 70:
      renderChar(452, 235, 23, 41, 4, 17, 28, coords);
      break;
     case 71:
      renderChar(35, 165, 34, 43, 2, 16, 39, coords);
      break;
     case 72:
      renderChar(0, 209, 33, 42, 4, 16, 40, coords);
      break;
     case 73:
      renderChar(428, 193, 6, 42, 4, 16, 14, coords);
      break;
     case 74:
      renderChar(6, 0, 17, 55, -6, 16, 14, coords);
      break;
     case 75:
      renderChar(135, 198, 29, 42, 4, 16, 32, coords);
      break;
     case 76:
      renderChar(354, 193, 24, 42, 4, 16, 28, coords);
      break;
     case 77:
      renderChar(389, 150, 43, 42, 4, 16, 50, coords);
      break;
     case 78:
      renderChar(68, 209, 33, 42, 3, 16, 39, coords);
      break;
     case 79:
      renderChar(394, 106, 40, 43, 1, 16, 42, coords);
      break;
     case 80:
      renderChar(276, 195, 25, 42, 4, 16, 30, coords);
      break;
     case 81:
      renderChar(24, 0, 40, 54, 1, 16, 42, coords);
      break;
     case 82:
      renderChar(222, 197, 26, 42, 4, 16, 31, coords);
      break;
     case 83:
      renderChar(215, 153, 26, 43, 2, 16, 30, coords);
      break;
     case 84:
      renderChar(0, 252, 32, 41, -1, 17, 30, coords);
      break;
     case 85:
      renderChar(70, 159, 32, 43, 3, 16, 38, coords);
      break;
     case 86:
      renderChar(470, 149, 35, 42, 0, 16, 35, coords);
      break;
     case 87:
      renderChar(336, 150, 52, 42, 0, 16, 52, coords);
      break;
     case 88:
      renderChar(102, 203, 32, 42, 0, 16, 32, coords);
      break;
     case 89:
      renderChar(34, 209, 33, 42, 0, 16, 32, coords);
      break;
     case 90:
      renderChar(33, 252, 30, 41, 1, 17, 32, coords);
      break;
     case 91:
      renderChar(435, 53, 12, 51, 5, 13, 21, coords);
      break;
     case 92:
      renderChar(353, 54, 25, 51, -1, 13, 24, coords);
      break;
     case 93:
      renderChar(422, 54, 12, 51, 4, 13, 21, coords);
      break;
     case 94:
      renderChar(452, 310, 22, 20, 3, 25, 28, coords);
      break;
     case 95:
      renderChar(37, 159, 32, 4, -1, 60, 30, coords);
      break;
     case 96:
      renderChar(73, 366, 11, 8, 8, 18, 30, coords);
      break;
     case 97:
      renderChar(24, 335, 23, 31, 2, 28, 28, coords);
      break;
     case 98:
      renderChar(172, 107, 28, 45, 3, 14, 33, coords);
      break;
     case 99:
      renderChar(0, 335, 23, 31, 2, 28, 26, coords);
      break;
     case 100:
      renderChar(201, 107, 27, 45, 2, 14, 33, coords);
      break;
     case 101:
      renderChar(475, 277, 24, 31, 2, 28, 28, coords);
      break;
     case 102:
      renderChar(229, 106, 21, 45, 1, 13, 18, coords);
      break;
     case 103:
      renderChar(132, 154, 27, 43, 2, 28, 33, coords);
      break;
     case 104:
      renderChar(251, 106, 25, 44, 3, 14, 31, coords);
      break;
     case 105:
      renderChar(504, 234, 7, 40, 3, 18, 13, coords);
      break;
     case 106:
      renderChar(353, 0, 15, 53, -4, 18, 14, coords);
      break;
     case 107:
      renderChar(277, 106, 24, 44, 3, 14, 27, coords);
      break;
     case 108:
      renderChar(302, 106, 7, 44, 3, 14, 13, coords);
      break;
     case 109:
      renderChar(69, 335, 41, 30, 3, 28, 47, coords);
      break;
     case 110:
      renderChar(167, 328, 25, 30, 3, 28, 31, coords);
      break;
     case 111:
      renderChar(446, 278, 28, 31, 2, 28, 31, coords);
      break;
     case 112:
      renderChar(103, 155, 28, 43, 3, 28, 33, coords);
      break;
     case 113:
      renderChar(160, 154, 27, 43, 2, 28, 33, coords);
      break;
     case 114:
      renderChar(245, 322, 16, 30, 3, 28, 19, coords);
      break;
     case 115:
      renderChar(48, 335, 20, 31, 1, 28, 22, coords);
      break;
     case 116:
      renderChar(353, 279, 18, 37, 1, 22, 20, coords);
      break;
     case 117:
      renderChar(193, 323, 25, 30, 3, 29, 31, coords);
      break;
     case 118:
      renderChar(303, 320, 28, 29, 0, 29, 27, coords);
      break;
     case 119:
      renderChar(262, 322, 40, 29, 0, 29, 39, coords);
      break;
     case 120:
      renderChar(332, 319, 25, 29, 0, 29, 26, coords);
      break;
     case 121:
      renderChar(165, 198, 28, 42, 0, 29, 27, coords);
      break;
     case 122:
      renderChar(358, 317, 22, 29, 1, 29, 24, coords);
      break;
     case 123:
      renderChar(397, 0, 13, 53, 4, 12, 21, coords);
      break;
     case 124:
      renderChar(0, 0, 5, 60, 6, 12, 18, coords);
      break;
     case 125:
      renderChar(383, 0, 13, 53, 4, 12, 21, coords);
      break;
     case 126:
      renderChar(43, 367, 29, 9, 2, 38, 33, coords);
      break;
     case 161:
      renderChar(326, 150, 9, 43, 3, 28, 15, coords);
      break;
     case 163:
      renderChar(486, 192, 25, 41, 3, 17, 29, coords);
      break;
     case 165:
      renderChar(476, 235, 27, 40, 1, 18, 29, coords);
      break;
     case 168:
      renderChar(106, 366, 16, 6, 7, 20, 30, coords);
      break;
     case 171:
      renderChar(407, 310, 22, 21, 2, 32, 26, coords);
      break;
     case 180:
      renderChar(85, 366, 11, 8, 9, 18, 30, coords);
      break;
     case 184:
      renderChar(30, 367, 12, 11, 9, 57, 30, coords);
      break;
     case 187:
      renderChar(430, 310, 21, 21, 3, 32, 26, coords);
      break;
     case 191:
      renderChar(268, 151, 23, 43, 1, 28, 24, coords);
      break;
     case 192:
      renderChar(461, 0, 36, 52, 0, 6, 36, coords);
      break;
     case 193:
      renderChar(0, 61, 36, 52, 0, 6, 36, coords);
      break;
     case 194:
      renderChar(424, 0, 36, 52, 0, 6, 36, coords);
      break;
     case 195:
      renderChar(0, 114, 36, 50, 0, 8, 36, coords);
      break;
     case 196:
      renderChar(448, 53, 36, 50, 0, 8, 36, coords);
      break;
     case 197:
      renderChar(188, 0, 36, 53, 0, 5, 36, coords);
      break;
     case 198:
      renderChar(435, 193, 50, 41, 0, 17, 51, coords);
      break;
     case 199:
      renderChar(37, 55, 34, 52, 2, 16, 37, coords);
      break;
     case 200:
      renderChar(156, 54, 24, 52, 4, 6, 29, coords);
      break;
     case 201:
      renderChar(106, 54, 24, 52, 4, 6, 29, coords);
      break;
     case 202:
      renderChar(131, 54, 24, 52, 4, 6, 29, coords);
      break;
     case 203:
      renderChar(485, 53, 24, 50, 4, 8, 29, coords);
      break;
     case 204:
      renderChar(498, 0, 12, 52, -2, 6, 14, coords);
      break;
     case 205:
      renderChar(197, 54, 11, 52, 4, 6, 14, coords);
      break;
     case 206:
      renderChar(181, 54, 15, 52, -1, 6, 14, coords);
      break;
     case 207:
      renderChar(405, 54, 16, 51, -1, 7, 14, coords);
      break;
     case 209:
      renderChar(37, 108, 33, 50, 3, 8, 39, coords);
      break;
     case 210:
      renderChar(106, 0, 40, 53, 1, 6, 42, coords);
      break;
     case 211:
      renderChar(147, 0, 40, 53, 1, 6, 42, coords);
      break;
     case 212:
      renderChar(65, 0, 40, 53, 1, 6, 42, coords);
      break;
     case 213:
      renderChar(209, 54, 40, 51, 1, 8, 42, coords);
      break;
     case 214:
      renderChar(250, 54, 40, 51, 1, 8, 42, coords);
      break;
     case 216:
      renderChar(353, 106, 40, 43, 1, 16, 42, coords);
      break;
     case 217:
      renderChar(225, 0, 32, 53, 3, 6, 38, coords);
      break;
     case 218:
      renderChar(258, 0, 32, 53, 3, 6, 38, coords);
      break;
     case 219:
      renderChar(291, 0, 32, 53, 3, 6, 38, coords);
      break;
     case 220:
      renderChar(291, 54, 32, 51, 3, 8, 38, coords);
      break;
     case 221:
      renderChar(72, 54, 33, 52, 0, 6, 32, coords);
      break;
     case 223:
      renderChar(117, 107, 30, 46, 2, 13, 34, coords);
      break;
     case 224:
      renderChar(380, 236, 23, 41, 2, 18, 28, coords);
      break;
     case 225:
      renderChar(404, 236, 23, 41, 2, 18, 28, coords);
      break;
     case 226:
      renderChar(428, 236, 23, 41, 2, 18, 28, coords);
      break;
     case 227:
      renderChar(248, 282, 23, 39, 2, 20, 28, coords);
      break;
     case 228:
      renderChar(272, 281, 23, 39, 2, 20, 28, coords);
      break;
     case 229:
      renderChar(404, 193, 23, 42, 2, 17, 28, coords);
      break;
     case 230:
      renderChar(404, 278, 41, 31, 2, 28, 45, coords);
      break;
     case 231:
      renderChar(55, 294, 23, 40, 2, 28, 26, coords);
      break;
     case 232:
      renderChar(255, 239, 24, 41, 2, 18, 28, coords);
      break;
     case 233:
      renderChar(330, 237, 24, 41, 2, 18, 28, coords);
      break;
     case 234:
      renderChar(305, 238, 24, 41, 2, 18, 28, coords);
      break;
     case 235:
      renderChar(223, 282, 24, 39, 2, 20, 28, coords);
      break;
     case 236:
      renderChar(113, 294, 12, 40, -2, 18, 13, coords);
      break;
     case 237:
      renderChar(126, 288, 12, 40, 3, 18, 13, coords);
      break;
     case 238:
      renderChar(97, 294, 15, 40, -1, 18, 13, coords);
      break;
     case 239:
      renderChar(335, 279, 17, 38, -2, 20, 13, coords);
      break;
     case 241:
      renderChar(309, 280, 25, 38, 3, 20, 31, coords);
      break;
     case 242:
      renderChar(93, 252, 28, 41, 2, 18, 31, coords);
      break;
     case 243:
      renderChar(64, 252, 28, 41, 2, 18, 31, coords);
      break;
     case 244:
      renderChar(122, 246, 28, 41, 2, 18, 31, coords);
      break;
     case 245:
      renderChar(139, 288, 28, 39, 2, 20, 31, coords);
      break;
     case 246:
      renderChar(168, 283, 28, 39, 2, 20, 31, coords);
      break;
     case 248:
      renderChar(372, 278, 31, 33, 0, 27, 31, coords);
      break;
     case 249:
      renderChar(177, 241, 25, 41, 3, 18, 31, coords);
      break;
     case 250:
      renderChar(203, 240, 25, 41, 3, 18, 31, coords);
      break;
     case 251:
      renderChar(229, 240, 25, 41, 3, 18, 31, coords);
      break;
     case 252:
      renderChar(197, 283, 25, 39, 3, 20, 31, coords);
      break;
     case 253:
      renderChar(324, 0, 28, 53, 0, 18, 27, coords);
      break;
     case 255:
      renderChar(324, 54, 28, 51, 0, 20, 27, coords);
      break;
    default:
      renderChar(292, 151, 23, 43, 1, 16, 25, coords);
      break;
    }
  }
}

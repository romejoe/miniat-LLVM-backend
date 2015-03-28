//===-- MiniATBaseInfo.h - Top level definitions for MINIAT MC ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone helper functions and enum definitions for
// the MiniAT target useful for the compiler back-end and the MC libraries.
//
//===----------------------------------------------------------------------===//
#ifndef MINIATBASEINFO_H
#define MINIATBASEINFO_H

#include "MiniATMCTargetDesc.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

/// MiniATII - This namespace holds all of the target specific flags that
/// instruction info tracks.
///
    namespace MiniATII {
        /// Target Operand Flag enum.
        enum TOF {
            //===------------------------------------------------------------------===//
            // MiniAT Specific MachineOperand flags.

            MO_NO_FLAG,

            /// MO_GOT_CALL - Represents the offset into the global offset table at
            /// which the address of a call site relocation entry symbol resides
            /// during execution. This is different from the above since this flag
            /// can only be present in call instructions.
                    MO_GOT_CALL,

            /// MO_GPREL - Represents the offset from the current gp value to be used
            /// for the relocatable object file being produced.
                    MO_GPREL,

            /// MO_ABS_HI/LO - Represents the hi or low part of an absolute symbol
            /// address.
                    MO_ABS_HI,
            MO_ABS_LO,

            MO_GOT_DISP,
            MO_GOT_PAGE,
            MO_GOT_OFST,

            // N32/64 Flags.
                    MO_GPOFF_HI,
            MO_GPOFF_LO,

            /// MO_GOT_HI16/LO16 - Relocations used for large GOTs.
                    MO_GOT_HI16,
            MO_GOT_LO16
        }; // enum TOF {

        enum {
            //===------------------------------------------------------------------===//
            // Instruction encodings.  These are the standard/most common forms for
            // MiniAT instructions.
            //

            // Pseudo - This represents an instruction that is a pseudo instruction
            // or one that has not been implemented yet.  It is illegal to code generate
            // it, but tolerated for intermediate implementation stages.
                    Pseudo   = 0,

            /// FrmR - This form is for instructions of the format R.
                    FrmR  = 1,
            /// FrmI - This form is for instructions of the format I.
                    FrmI  = 2,
            /// FrmJ - This form is for instructions of the format J.
                    FrmJ  = 3,
            /// FrmOther - This form is for instructions that have no specific format.
                    FrmOther = 4,

            FormMask = 15
        };
    }

/// getMiniATRegisterNumbering - Given the enum value for some register,
/// return the number that it corresponds to.
    inline static unsigned getMiniATRegisterNumbering(unsigned RegEnum)
    {
        //TODO: Find a better way to do this, too much code to maintain
        switch (RegEnum) {
            case MiniAT::r0:
                return 0;
            case MiniAT::r1:
                return 1;
            case MiniAT::r2:
                return 2;
            case MiniAT::r3:
                return 3;
            case MiniAT::r4:
                return 4;
            case MiniAT::r5:
                return 5;
            case MiniAT::r6:
                return 6;
            case MiniAT::r7:
                return 7;
            case MiniAT::r8:
                return 8;
            case MiniAT::r9:
                return 9;
            case MiniAT::r10:
                return 10;
            case MiniAT::r11:
                return 11;
            case MiniAT::r12:
                return 12;
            case MiniAT::r13:
                return 13;
            case MiniAT::r14:
                return 14;
            case MiniAT::r15:
                return 15;
            case MiniAT::r16:
                return 16;
            case MiniAT::r17:
                return 17;
            case MiniAT::r18:
                return 18;
            case MiniAT::r19:
                return 19;
            case MiniAT::r20:
                return 20;
            case MiniAT::r21:
                return 21;
            case MiniAT::r22:
                return 22;
            case MiniAT::r23:
                return 23;
            case MiniAT::r24:
                return 24;
            case MiniAT::r25:
                return 25;
            case MiniAT::r26:
                return 26;
            case MiniAT::r27:
                return 27;
            case MiniAT::r28:
                return 28;
            case MiniAT::r29:
                return 29;
            case MiniAT::r30:
                return 30;
            case MiniAT::r31:
                return 31;
            case MiniAT::r32:
                return 32;
            case MiniAT::r33:
                return 33;
            case MiniAT::r34:
                return 34;
            case MiniAT::r35:
                return 35;
            case MiniAT::r36:
                return 36;
            case MiniAT::r37:
                return 37;
            case MiniAT::r38:
                return 38;
            case MiniAT::r39:
                return 39;
            case MiniAT::r40:
                return 40;
            case MiniAT::r41:
                return 41;
            case MiniAT::r42:
                return 42;
            case MiniAT::r43:
                return 43;
            case MiniAT::r44:
                return 44;
            case MiniAT::r45:
                return 45;
            case MiniAT::r46:
                return 46;
            case MiniAT::r47:
                return 47;
            case MiniAT::r48:
                return 48;
            case MiniAT::r49:
                return 49;
            case MiniAT::r50:
                return 50;
            case MiniAT::r51:
                return 51;
            case MiniAT::r52:
                return 52;
            case MiniAT::r53:
                return 53;
            case MiniAT::r54:
                return 54;
            case MiniAT::r55:
                return 55;
            case MiniAT::r56:
                return 56;
            case MiniAT::r57:
                return 57;
            case MiniAT::r58:
                return 58;
            case MiniAT::r59:
                return 59;
            case MiniAT::r60:
                return 60;
            case MiniAT::r61:
                return 61;
            case MiniAT::r62:
                return 62;
            case MiniAT::r63:
                return 63;
            case MiniAT::r64:
                return 64;
            case MiniAT::r65:
                return 65;
            case MiniAT::r66:
                return 66;
            case MiniAT::r67:
                return 67;
            case MiniAT::r68:
                return 68;
            case MiniAT::r69:
                return 69;
            case MiniAT::r70:
                return 70;
            case MiniAT::r71:
                return 71;
            case MiniAT::r72:
                return 72;
            case MiniAT::r73:
                return 73;
            case MiniAT::r74:
                return 74;
            case MiniAT::r75:
                return 75;
            case MiniAT::r76:
                return 76;
            case MiniAT::r77:
                return 77;
            case MiniAT::r78:
                return 78;
            case MiniAT::r79:
                return 79;
            case MiniAT::r80:
                return 80;
            case MiniAT::r81:
                return 81;
            case MiniAT::r82:
                return 82;
            case MiniAT::r83:
                return 83;
            case MiniAT::r84:
                return 84;
            case MiniAT::r85:
                return 85;
            case MiniAT::r86:
                return 86;
            case MiniAT::r87:
                return 87;
            case MiniAT::r88:
                return 88;
            case MiniAT::r89:
                return 89;
            case MiniAT::r90:
                return 90;
            case MiniAT::r91:
                return 91;
            case MiniAT::r92:
                return 92;
            case MiniAT::r93:
                return 93;
            case MiniAT::r94:
                return 94;
            case MiniAT::r95:
                return 95;
            case MiniAT::r96:
                return 96;
            case MiniAT::r97:
                return 97;
            case MiniAT::r98:
                return 98;
            case MiniAT::r99:
                return 99;
            case MiniAT::r100:
                return 100;
            case MiniAT::r101:
                return 101;
            case MiniAT::r102:
                return 102;
            case MiniAT::r103:
                return 103;
            case MiniAT::r104:
                return 104;
            case MiniAT::r105:
                return 105;
            case MiniAT::r106:
                return 106;
            case MiniAT::r107:
                return 107;
            case MiniAT::r108:
                return 108;
            case MiniAT::r109:
                return 109;
            case MiniAT::r110:
                return 110;
            case MiniAT::r111:
                return 111;
            case MiniAT::r112:
                return 112;
            case MiniAT::r113:
                return 113;
            case MiniAT::r114:
                return 114;
            case MiniAT::r115:
                return 115;
            case MiniAT::r116:
                return 116;
            case MiniAT::r117:
                return 117;
            case MiniAT::r118:
                return 118;
            case MiniAT::r119:
                return 119;
            case MiniAT::r120:
                return 120;
            case MiniAT::r121:
                return 121;
            case MiniAT::r122:
                return 122;
            case MiniAT::r123:
                return 123;
            case MiniAT::r124:
                return 124;
            case MiniAT::r125:
                return 125;
            case MiniAT::r126:
                return 126;
            case MiniAT::r127:
                return 127;
            case MiniAT::r128:
                return 128;
            case MiniAT::r129:
                return 129;
            case MiniAT::r130:
                return 130;
            case MiniAT::r131:
                return 131;
            case MiniAT::r132:
                return 132;
            case MiniAT::r133:
                return 133;
            case MiniAT::r134:
                return 134;
            case MiniAT::r135:
                return 135;
            case MiniAT::r136:
                return 136;
            case MiniAT::r137:
                return 137;
            case MiniAT::r138:
                return 138;
            case MiniAT::r139:
                return 139;
            case MiniAT::r140:
                return 140;
            case MiniAT::r141:
                return 141;
            case MiniAT::r142:
                return 142;
            case MiniAT::r143:
                return 143;
            case MiniAT::r144:
                return 144;
            case MiniAT::r145:
                return 145;
            case MiniAT::r146:
                return 146;
            case MiniAT::r147:
                return 147;
            case MiniAT::r148:
                return 148;
            case MiniAT::r149:
                return 149;
            case MiniAT::r150:
                return 150;
            case MiniAT::r151:
                return 151;
            case MiniAT::r152:
                return 152;
            case MiniAT::r153:
                return 153;
            case MiniAT::r154:
                return 154;
            case MiniAT::r155:
                return 155;
            case MiniAT::r156:
                return 156;
            case MiniAT::r157:
                return 157;
            case MiniAT::r158:
                return 158;
            case MiniAT::r159:
                return 159;
            case MiniAT::r160:
                return 160;
            case MiniAT::r161:
                return 161;
            case MiniAT::r162:
                return 162;
            case MiniAT::r163:
                return 163;
            case MiniAT::r164:
                return 164;
            case MiniAT::r165:
                return 165;
            case MiniAT::r166:
                return 166;
            case MiniAT::r167:
                return 167;
            case MiniAT::r168:
                return 168;
            case MiniAT::r169:
                return 169;
            case MiniAT::r170:
                return 170;
            case MiniAT::r171:
                return 171;
            case MiniAT::r172:
                return 172;
            case MiniAT::r173:
                return 173;
            case MiniAT::r174:
                return 174;
            case MiniAT::r175:
                return 175;
            case MiniAT::r176:
                return 176;
            case MiniAT::r177:
                return 177;
            case MiniAT::r178:
                return 178;
            case MiniAT::r179:
                return 179;
            case MiniAT::r180:
                return 180;
            case MiniAT::r181:
                return 181;
            case MiniAT::r182:
                return 182;
            case MiniAT::r183:
                return 183;
            case MiniAT::r184:
                return 184;
            case MiniAT::r185:
                return 185;
            case MiniAT::r186:
                return 186;
            case MiniAT::r187:
                return 187;
            case MiniAT::r188:
                return 188;
            case MiniAT::r189:
                return 189;
            case MiniAT::r190:
                return 190;
            case MiniAT::r191:
                return 191;
            case MiniAT::r192:
                return 192;
            case MiniAT::r193:
                return 193;
            case MiniAT::r194:
                return 194;
            case MiniAT::r195:
                return 195;
            case MiniAT::r196:
                return 196;
            case MiniAT::r197:
                return 197;
            case MiniAT::r198:
                return 198;
            case MiniAT::r199:
                return 199;
            case MiniAT::r200:
                return 200;
            case MiniAT::r201:
                return 201;
            case MiniAT::r202:
                return 202;
            case MiniAT::r203:
                return 203;
            case MiniAT::r204:
                return 204;
            case MiniAT::r205:
                return 205;
            case MiniAT::r206:
                return 206;
            case MiniAT::r207:
                return 207;
            case MiniAT::r208:
                return 208;
            case MiniAT::r209:
                return 209;
            case MiniAT::r210:
                return 210;
            case MiniAT::r211:
                return 211;
            case MiniAT::r212:
                return 212;
            case MiniAT::r213:
                return 213;
            case MiniAT::r214:
                return 214;
            case MiniAT::r215:
                return 215;
            case MiniAT::r216:
                return 216;
            case MiniAT::r217:
                return 217;
            case MiniAT::r218:
                return 218;
            case MiniAT::r219:
                return 219;
            case MiniAT::r220:
                return 220;
            case MiniAT::r221:
                return 221;
            case MiniAT::r222:
                return 222;
            case MiniAT::r223:
                return 223;
            case MiniAT::r224:
                return 224;
            case MiniAT::r225:
                return 225;
            case MiniAT::r226:
                return 226;
            case MiniAT::r227:
                return 227;
            case MiniAT::r228:
                return 228;
            case MiniAT::r229:
                return 229;
            case MiniAT::r230:
                return 230;
            case MiniAT::r231:
                return 231;
            case MiniAT::r232:
                return 232;
            case MiniAT::r233:
                return 233;
            case MiniAT::r234:
                return 234;
            case MiniAT::r235:
                return 235;
            case MiniAT::r236:
                return 236;
            case MiniAT::r237:
                return 237;
            case MiniAT::r238:
                return 238;
            case MiniAT::r239:
                return 239;
            case MiniAT::r240:
                return 240;
            case MiniAT::r241:
                return 241;
            case MiniAT::r242:
                return 242;
            case MiniAT::r243:
                return 243;
            case MiniAT::r244:
                return 244;
            case MiniAT::r245:
                return 245;
            case MiniAT::r246:
                return 246;
            case MiniAT::r247:
                return 247;
            case MiniAT::r248:
                return 248;
            case MiniAT::r249:
                return 249;
            case MiniAT::r250:
                return 250;
            case MiniAT::r251:
                return 251;
            case MiniAT::r252:
                return 252;
            case MiniAT::r253:
                return 253;
            case MiniAT::r254:
                return 254;
            case MiniAT::r255:
                return 255;
            default: llvm_unreachable("Unknown register number!");
        }
    }

}

#endif
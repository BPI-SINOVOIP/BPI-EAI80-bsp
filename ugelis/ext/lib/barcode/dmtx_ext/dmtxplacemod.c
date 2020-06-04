/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2008, 2009 Mike Laughton. All rights reserved.
 * Copyright 2012-2016 Vadim A. Misbakh-Soloviov. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact:
 * Vadim A. Misbakh-Soloviov <dmtx@mva.name>
 * Mike Laughton <mike@dragonflylogic.com>
 *
 * \file dmtxplacemod.c
 * \brief Data Matrix module placement
 */

/**
 * receives symbol row and col and returns status
 * DmtxModuleOn / !DmtxModuleOn (DmtxModuleOff)
 * DmtxModuleAssigned
 * DmtxModuleVisited
 * DmtxModuleData / !DmtxModuleData (DmtxModuleAlignment)
 * row and col are expressed in symbol coordinates, so (0,0) is the intersection of the "L"
 */
int
dmtxSymbolModuleStatus(DmtxMessage *message, int sizeIdx, int symbolRow, int symbolCol)
{
   int symbolRowReverse;
   int mappingRow, mappingCol;
   int dataRegionRows, dataRegionCols;
   int symbolRows, mappingCols;

   dataRegionRows = dmtxGetSymbolAttribute(DmtxSymAttribDataRegionRows, sizeIdx);
   dataRegionCols = dmtxGetSymbolAttribute(DmtxSymAttribDataRegionCols, sizeIdx);
   symbolRows = dmtxGetSymbolAttribute(DmtxSymAttribSymbolRows, sizeIdx);
   mappingCols = dmtxGetSymbolAttribute(DmtxSymAttribMappingMatrixCols, sizeIdx);

   symbolRowReverse = symbolRows - symbolRow - 1;
   mappingRow = symbolRowReverse - 1 - 2 * (symbolRowReverse / (dataRegionRows+2));
   mappingCol = symbolCol - 1 - 2 * (symbolCol / (dataRegionCols+2));

   /* Solid portion of alignment patterns */
   if(symbolRow % (dataRegionRows+2) == 0 ||
         symbolCol % (dataRegionCols+2) == 0)
      return (DmtxModuleOnRGB | (!DmtxModuleData));

   /* Horinzontal calibration bars */
   if((symbolRow+1) % (dataRegionRows+2) == 0)
      return (((symbolCol & 0x01) ? 0 : DmtxModuleOnRGB) | (!DmtxModuleData));

   /* Vertical calibration bars */
   if((symbolCol+1) % (dataRegionCols+2) == 0)
      return (((symbolRow & 0x01) ? 0 : DmtxModuleOnRGB) | (!DmtxModuleData));

   /* Data modules */
   return (message->array[mappingRow * mappingCols + mappingCol] | DmtxModuleData);
}

/**
 * \brief  Logical relationship between bit and module locations
 * \param  modules
 * \param  codewords
 * \param  sizeIdx
 * \param  moduleOnColor
 * \return Number of codewords read
 */
static int
ModulePlacementEcc200(unsigned char *modules, unsigned char *codewords, int sizeIdx, int moduleOnColor)
{
   int row, col, chr;
   int mappingRows, mappingCols;

   assert(moduleOnColor & (DmtxModuleOnRed | DmtxModuleOnGreen | DmtxModuleOnBlue));

   mappingRows = dmtxGetSymbolAttribute(DmtxSymAttribMappingMatrixRows, sizeIdx);
   mappingCols = dmtxGetSymbolAttribute(DmtxSymAttribMappingMatrixCols, sizeIdx);

   /* Start in the nominal location for the 8th bit of the first character */
   chr = 0;
   row = 4;
   col = 0;

   do {
      /* Repeatedly first check for one of the special corner cases */
      if((row == mappingRows) && (col == 0))
         PatternShapeSpecial1(modules, mappingRows, mappingCols, &(codewords[chr++]), moduleOnColor);
      else if((row == mappingRows-2) && (col == 0) && (mappingCols%4 != 0))
         PatternShapeSpecial2(modules, mappingRows, mappingCols, &(codewords[chr++]), moduleOnColor);
      else if((row == mappingRows-2) && (col == 0) && (mappingCols%8 == 4))
         PatternShapeSpecial3(modules, mappingRows, mappingCols, &(codewords[chr++]), moduleOnColor);
      else if((row == mappingRows+4) && (col == 2) && (mappingCols%8 == 0))
         PatternShapeSpecial4(modules, mappingRows, mappingCols, &(codewords[chr++]), moduleOnColor);

      /* Sweep upward diagonally, inserting successive characters */
      do {
         if((row < mappingRows) && (col >= 0) &&
               !(modules[row*mappingCols+col] & DmtxModuleVisited))
            PatternShapeStandard(modules, mappingRows, mappingCols, row, col, &(codewords[chr++]), moduleOnColor);
         row -= 2;
         col += 2;
      } while ((row >= 0) && (col < mappingCols));
      row += 1;
      col += 3;

      /* Sweep downward diagonally, inserting successive characters */
      do {
         if((row >= 0) && (col < mappingCols) &&
               !(modules[row*mappingCols+col] & DmtxModuleVisited))
            PatternShapeStandard(modules, mappingRows, mappingCols, row, col, &(codewords[chr++]), moduleOnColor);
         row += 2;
         col -= 2;
      } while ((row < mappingRows) && (col >= 0));
      row += 3;
      col += 1;
      /* ... until the entire modules array is scanned */
   } while ((row < mappingRows) || (col < mappingCols));

   /* If lower righthand corner is untouched then fill in the fixed pattern */
   if(!(modules[mappingRows * mappingCols - 1] &
         DmtxModuleVisited)) {

      modules[mappingRows * mappingCols - 1] |= moduleOnColor;
      modules[(mappingRows * mappingCols) - mappingCols - 2] |= moduleOnColor;
   } /* XXX should this fixed pattern also be used in reading somehow? */

   /* XXX compare that chr == region->dataSize here */
   return chr; /* XXX number of codewords read off */
}

/**
 * \brief  XXX
 * \param  modules
 * \param  mappingRows
 * \param  mappingCols
 * \param  row
 * \param  col
 * \param  codeword
 * \param  moduleOnColor
 * \return void
 */
static void
PatternShapeStandard(unsigned char *modules, int mappingRows, int mappingCols, int row, int col, unsigned char *codeword, int moduleOnColor)
{
   PlaceModule(modules, mappingRows, mappingCols, row-2, col-2, codeword, DmtxMaskBit1, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, row-2, col-1, codeword, DmtxMaskBit2, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, row-1, col-2, codeword, DmtxMaskBit3, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, row-1, col-1, codeword, DmtxMaskBit4, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, row-1, col,   codeword, DmtxMaskBit5, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, row,   col-2, codeword, DmtxMaskBit6, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, row,   col-1, codeword, DmtxMaskBit7, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, row,   col,   codeword, DmtxMaskBit8, moduleOnColor);
}

/**
 * \brief  XXX
 * \param  modules
 * \param  mappingRows
 * \param  mappingCols
 * \param  codeword
 * \param  moduleOnColor
 * \return void
 */
static void
PatternShapeSpecial1(unsigned char *modules, int mappingRows, int mappingCols, unsigned char *codeword, int moduleOnColor)
{
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-1, 0, codeword, DmtxMaskBit1, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-1, 1, codeword, DmtxMaskBit2, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-1, 2, codeword, DmtxMaskBit3, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-2, codeword, DmtxMaskBit4, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-1, codeword, DmtxMaskBit5, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 1, mappingCols-1, codeword, DmtxMaskBit6, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 2, mappingCols-1, codeword, DmtxMaskBit7, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 3, mappingCols-1, codeword, DmtxMaskBit8, moduleOnColor);
}

/**
 * \brief  XXX
 * \param  modules
 * \param  mappingRows
 * \param  mappingCols
 * \param  codeword
 * \param  moduleOnColor
 * \return void
 */
static void
PatternShapeSpecial2(unsigned char *modules, int mappingRows, int mappingCols, unsigned char *codeword, int moduleOnColor)
{
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-3, 0, codeword, DmtxMaskBit1, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-2, 0, codeword, DmtxMaskBit2, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-1, 0, codeword, DmtxMaskBit3, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-4, codeword, DmtxMaskBit4, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-3, codeword, DmtxMaskBit5, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-2, codeword, DmtxMaskBit6, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-1, codeword, DmtxMaskBit7, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 1, mappingCols-1, codeword, DmtxMaskBit8, moduleOnColor);
}

/**
 * \brief  XXX
 * \param  modules
 * \param  mappingRows
 * \param  mappingCols
 * \param  codeword
 * \param  moduleOnColor
 * \return void
 */
static void
PatternShapeSpecial3(unsigned char *modules, int mappingRows, int mappingCols, unsigned char *codeword, int moduleOnColor)
{
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-3, 0, codeword, DmtxMaskBit1, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-2, 0, codeword, DmtxMaskBit2, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-1, 0, codeword, DmtxMaskBit3, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-2, codeword, DmtxMaskBit4, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-1, codeword, DmtxMaskBit5, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 1, mappingCols-1, codeword, DmtxMaskBit6, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 2, mappingCols-1, codeword, DmtxMaskBit7, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 3, mappingCols-1, codeword, DmtxMaskBit8, moduleOnColor);
}

/**
 * \brief  XXX
 * \param  modules
 * \param  mappingRows
 * \param  mappingCols
 * \param  codeword
 * \param  moduleOnColor
 * \return void
 */
static void
PatternShapeSpecial4(unsigned char *modules, int mappingRows, int mappingCols, unsigned char *codeword, int moduleOnColor)
{
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-1, 0, codeword, DmtxMaskBit1, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, mappingRows-1, mappingCols-1, codeword, DmtxMaskBit2, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-3, codeword, DmtxMaskBit3, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-2, codeword, DmtxMaskBit4, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 0, mappingCols-1, codeword, DmtxMaskBit5, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 1, mappingCols-3, codeword, DmtxMaskBit6, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 1, mappingCols-2, codeword, DmtxMaskBit7, moduleOnColor);
   PlaceModule(modules, mappingRows, mappingCols, 1, mappingCols-1, codeword, DmtxMaskBit8, moduleOnColor);
}

/**
 * \brief  XXX
 * \param  modules
 * \param  mappingRows
 * \param  mappingCols
 * \param  row
 * \param  col
 * \param  codeword
 * \param  mask
 * \param  moduleOnColor
 * \return void
 */
static void
PlaceModule(unsigned char *modules, int mappingRows, int mappingCols, int row, int col, unsigned char *codeword, int mask, int moduleOnColor)
{
   if(row < 0) {
      row += mappingRows;
      col += 4 - ((mappingRows+4)%8);
   }
   if(col < 0) {
      col += mappingCols;
      row += 4 - ((mappingCols+4)%8);
   }

   /* If module has already been assigned then we are decoding the pattern into codewords */
   if((modules[row*mappingCols+col] & DmtxModuleAssigned) != 0) {
      if((modules[row*mappingCols+col] & moduleOnColor) != 0)
         *codeword |= mask;
      else
         *codeword &= (0xff ^ mask);
   }
   /* Otherwise we are encoding the codewords into a pattern */
   else {
      if((*codeword & mask) != 0x00)
         modules[row*mappingCols+col] |= moduleOnColor;

      modules[row*mappingCols+col] |= DmtxModuleAssigned;
   }

   modules[row*mappingCols+col] |= DmtxModuleVisited;
}

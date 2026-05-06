#include "TextRasterizer.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "TextCharset.h"

CTextRasterizer::CTextRasterizer(uint32_t uiWidth, uint32_t uiHeight) {
    m_uiBitmapWidth = uiWidth;
    m_uiBitmapHeight = uiHeight;
    m_uiBitmapSize = m_uiBitmapWidth * m_uiBitmapHeight;

    // اصلاح calloc
    m_pBitmap = (uint32_t*)calloc(m_uiBitmapSize, sizeof(uint32_t));
    memset(&m_stbFont, 0, sizeof(stbtt_fontinfo));
    m_bFontInitialised = false;
}

CTextRasterizer::~CTextRasterizer() {
    FreeFont();
    FreeBitmap();
    m_bFontInitialised = false;
}

uint8_t* CTextRasterizer::GetBitmap() {
    return (uint8_t*)m_pBitmap;
}

void CTextRasterizer::FreeBitmap() {
    if (m_pBitmap) {
        free(m_pBitmap);
        m_pBitmap = nullptr;
    }
}

void CTextRasterizer::SetupFont(const std::string& sFontPath, float fFontSize) {
    FILE* pFile = fopen(sFontPath.c_str(), "rb");
    if (!pFile) return;

    fseek(pFile, 0, SEEK_END);
    m_lFontBufferSize = ftell(pFile);
    rewind(pFile);

    m_pFontBuffer = (uint8_t*)malloc(m_lFontBufferSize);
    if (!m_pFontBuffer) {
        fclose(pFile);
        return;
    }

    fread(m_pFontBuffer, m_lFontBufferSize, 1, pFile);
    fclose(pFile);

    int iFontOffset = stbtt_GetFontOffsetForIndex(m_pFontBuffer, 0);
    stbtt_InitFont(&m_stbFont, m_pFontBuffer, iFontOffset);

    SetFontSize(fFontSize);
    SetPosition(0, 0);

    m_bFontInitialised = true;
}

void CTextRasterizer::SetFontSize(float fSize) {
    m_fFontSize = stbtt_ScaleForPixelHeight(&m_stbFont, fSize);
    stbtt_GetFontVMetrics(&m_stbFont, &m_iLineAscent, 0, 0);
}

uint8_t* CTextRasterizer::GetFont() {
    return (uint8_t*)m_pFontBuffer;
}

void CTextRasterizer::FreeFont() {
    if (m_pFontBuffer) {
        free(m_pFontBuffer);
        m_pFontBuffer = nullptr;
    }
}

void CTextRasterizer::SetPosition(uint32_t uiX, uint32_t uiY) {
    if (uiX != __IGNORE_VALUE__) m_uiCurrentX = m_uiPositionX = uiX;
    if (uiY != __IGNORE_VALUE__) m_uiCurrentY = uiY;
}

void CTextRasterizer::SetTextAlignment(uint8_t ucTextAlignment) {
    m_ucTextAlignment = ucTextAlignment;
}

void CTextRasterizer::DrawText_(CColouredTextMultiLine& colouredText) {
    switch (m_ucTextAlignment) {
        case TextAlignmentLeft:
            SetPosition(__IGNORE_VALUE__, 0); break;
        case TextAlignmentCenter:
            SetPosition(__IGNORE_VALUE__, m_uiBitmapHeight / 2 - GetTextDrawHeight(colouredText.GetLinesCount()) / 2); break;
        case TextAlignmentRight:
            SetPosition(__IGNORE_VALUE__, 0); break;
    }

    colouredText.Foreach([&](size_t uiCurrentLineId, CColouredTextLine& line) {
        switch (m_ucTextAlignment) {
            case TextAlignmentLeft: SetPosition(0, __IGNORE_VALUE__); break;
            case TextAlignmentCenter: SetPosition(m_uiBitmapWidth / 2 - GetTextDrawLength(line.GetPlainText()) / 2, __IGNORE_VALUE__); break;
            case TextAlignmentRight: SetPosition(m_uiBitmapWidth - GetTextDrawLength(line.GetPlainText()), __IGNORE_VALUE__); break;
        }

        line.Foreach([&](size_t uiCurrentSectionId, CColouredTextSection& section) {
            DrawTextLine(section.GetText(), section.GetTextColour(), (uiCurrentLineId != 0 && uiCurrentSectionId == 0));
        });
    });
}

void CTextRasterizer::FillBitmapWithColor(CColor& colValue) {
    if (!m_pBitmap) return;
    for (uint32_t y = 0; y < m_uiBitmapHeight; y++) {
        for (uint32_t x = 0; x < m_uiBitmapWidth; x++) {
            m_pBitmap[y * m_uiBitmapWidth + x] = colValue.Get(CColor::eColorEndianness::COLOR_ENDIAN_ABGR);
        }
    }
}

void CTextRasterizer::DrawBitmap(const uint8_t* pSource, uint32_t uiDestX, uint32_t uiDestY,
                                 uint32_t uiSrcX, uint32_t uiSrcY, uint32_t uiWidth, uint32_t uiHeight,
                                 uint32_t uiDestStride, uint32_t uiSourceStride, CColor& colValue) {
    if (!m_pBitmap || !pSource) return;

    for (uint32_t y = 0; y < uiHeight; y++) {
        if (uiDestY + y >= m_uiBitmapHeight) break;
        for (uint32_t x = 0; x < uiWidth; x++) {
            if (uiDestX + x >= m_uiBitmapWidth) break;

            uint32_t destOffset = (uiDestY + y) * uiDestStride + uiDestX + x;
            uint32_t srcOffset = (uiSrcY + y) * uiSourceStride + uiSrcX + x;
            if (destOffset >= m_uiBitmapSize) continue;

            uint8_t pixel = pSource[srcOffset];
            if (pixel > 0) {
                CColor bg{ CColor::eColorEndianness::COLOR_ENDIAN_ABGR, m_pBitmap[destOffset] };
                float fGray = pixel / 255.0f;
                float fAlpha = colValue.GetAlpha() / 255.0f;
                float fOneMinusAlpha = 1.0f - fAlpha;
                CColor out{
                    (CColor::ColorChannelValue)((bg.GetRed() * fOneMinusAlpha) + fAlpha * fGray * colValue.GetRed()),
                    (CColor::ColorChannelValue)((bg.GetGreen() * fOneMinusAlpha) + fAlpha * fGray * colValue.GetGreen()),
                    (CColor::ColorChannelValue)((bg.GetBlue() * fOneMinusAlpha) + fAlpha * fGray * colValue.GetBlue()),
                    (CColor::ColorChannelValue)((bg.GetAlpha() * fOneMinusAlpha) + colValue.GetAlpha())
                };
                m_pBitmap[destOffset] = out.Get(CColor::eColorEndianness::COLOR_ENDIAN_ABGR);
            }
        }
    }
}

uint32_t CTextRasterizer::GetTextDrawLength(const std::string& szInput) {
    uint32_t len = 0;
    std::u32string u32s = CTextCharset::CP1251ToUTF32(szInput);
    if (!u32s.empty()) {
        for (size_t i = 0; u32s[i] != '\0'; i++)
            len += GetGlyphDrawLength(u32s[i], u32s[i + 1]);
    }
    return len;
}

uint32_t CTextRasterizer::GetTextDrawHeight(uint32_t lines) {
    return roundf((float)(lines * m_iLineAscent) * m_fFontSize * 1.2f);
}

uint32_t CTextRasterizer::GetGlyphDrawLength(char32_t c, char32_t next) {
    int adv = 0;
    stbtt_GetCodepointHMetrics(&m_stbFont, c, &adv, 0);
    float len = adv * m_fFontSize;
    if (next) len += stbtt_GetCodepointKernAdvance(&m_stbFont, c, next) * m_fFontSize;
    return (uint32_t)len;
}

void CTextRasterizer::DrawTextLine(const std::string& szInput, CColor& colValue, bool bFromNewLine) {
    if (!m_bFontInitialised || !m_pBitmap) return;

    int curX = m_uiCurrentX;
    int curY = m_uiCurrentY;
    if (bFromNewLine) {
        curX = m_uiPositionX;
        curY += GetTextDrawHeight(1);
    }

    std::u32string u32s = CTextCharset::CP1251ToUTF32(szInput);
    for (size_t i = 0; u32s[i] != '\0'; i++) {
        int glyphW = 0, glyphH = 0, offX = 0, offY = 0;
        uint8_t* glyph = stbtt_GetCodepointBitmap(&m_stbFont, m_fFontSize, m_fFontSize, u32s[i], &glyphW, &glyphH, &offX, &offY);
        if (!glyph) continue;

        DrawBitmap(glyph, curX + offX, curY + m_iLineAscent * m_fFontSize + offY,
                   0, 0, glyphW, glyphH, m_uiBitmapWidth, glyphW, colValue);
        curX += GetGlyphDrawLength(u32s[i], u32s[i + 1]);

        free(glyph); // جلوگیری از memory leak
    }

    m_uiCurrentX = curX;
    m_uiCurrentY = curY;
}

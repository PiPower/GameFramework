#include "ImageFile.h"
#include <objidl.h>
#include <gdiplusenums.h>
#include <gdiplus.h>
#include <assert.h>

#pragma comment (lib,"Gdiplus.lib")

ImageFile::ImageFile(unsigned int* FileBuff, int width, int height)
    :
    width(width), height(height)
{
    this->FileBuff = FileBuff;
}

ImageFile::ImageFile(const std::wstring filename)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;
    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
    {
        Gdiplus::Bitmap bmp(filename.c_str());

        if (bmp.GetLastStatus() != Gdiplus::Status::Ok)
        {
            OutputDebugString(L"Image File is not working");
        }


        height = bmp.GetHeight();
        width = bmp.GetWidth();

        FileBuff = new unsigned int[width * height];

        for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++)
            {
                Gdiplus::Color col;
                bmp.GetPixel(x, y, &col);
                FileBuff[y * width + x] = col.GetValue();
            }

    }
    Gdiplus::GdiplusShutdown(gdiplusToken);
}

unsigned int ImageFile::GetColorAt(int x, int y) const
{
    return FileBuff[y * width + x];
}

unsigned int* const ImageFile::GetFilePtr() const
{
    return FileBuff;
}

const int ImageFile::GetWidth() const noexcept
{
    return width;
}

const int ImageFile::GetHeight() const noexcept
{
    return height;
}

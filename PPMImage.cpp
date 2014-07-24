
#include "PPMImage.h"

PPMImage::PPMImage(const char *filename) :
image_(NULL)
{
    loadPPM(filename);
}

bool PPMImage::loadPPM(const char *filename)
{
    if (image_) {
        delete [] image_;
        image_ = NULL;
    }
    FILE *f = fopen(filename, "r");
    if (f == NULL) return false;
    char header_buf[256];
    const char *line = readLine(f, header_buf, sizeof(header_buf));
#define EXIT_WITH_MSG(m) { fprintf(stderr, "%s: %s |%s", filename, m, line); \
            fclose(f); return false; }
    if (sscanf(line, "P6 ") == EOF)
    EXIT_WITH_MSG("Can only handle P6 as PPM type.");
    line = readLine(f, header_buf, sizeof(header_buf));
    if (!line || sscanf(line, "%d %d ", &width_, &height_) != 2)
    EXIT_WITH_MSG("Width/height expected");
    int value;
    line = readLine(f, header_buf, sizeof(header_buf));
    if (!line || sscanf(line, "%d ", &value) != 1 || value != 255)
    EXIT_WITH_MSG("Only 255 for maxval allowed.");
    const size_t pixel_count = width_ * height_;
    image_ = new Pixel [ pixel_count ];
    assert(sizeof(Pixel) == 3);   // we make that assumption.
    if (fread(image_, sizeof(Pixel), pixel_count, f) != pixel_count) {
        line = "";
        EXIT_WITH_MSG("Not enough pixels read.");
    }
#undef EXIT_WITH_MSG
    fclose(f);
    fprintf(stderr, "Read image with %dx%d\n", width_, height_);
    return true;
}

const PPMImage::Pixel& PPMImage::getPixel(int x, int y)
{
    static Pixel dummy;
    if (x < 0 || x > width_ || y < 0 || y > height_) return dummy;
    return image_[x + width_ * y];
}

char * PPMImage::readLine(FILE *f, char *buffer, size_t len)
{
    char *result;
    do {
        result = fgets(buffer, len, f);
    } while (result != NULL && result[0] == '#');
    return result;
}

void PPMImage::drawImage(RGBMatrix* matrix,int pos_x , int pos_y)
{
    for (int x = 0; x < width_; ++x) {
        for (int y = 0; y < height_; ++y) {
            
            const Pixel &p = getPixel(x, y);
	    
	    if(p.red == 255 && p.green ==0 && p.blue == 255) continue;


            matrix->SetPixel(pos_x + x, pos_y +y, p.red, p.green, p.blue);
        }
    }
}

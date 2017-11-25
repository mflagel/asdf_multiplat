#include "stdafx.h"
#include "text.h"

using namespace std;

namespace asdf
{
    void text_info_t::align(glm::vec2 parent_halfsize)
    {
        if(face_size == 18)
            int asdf = 9001;

        //todo: use proper text metrics instead of bounding box (at least for height)
        font->FaceSize(face_size);
        auto bounding_box = font->BBox(str.c_str()); //just grabing width/height
        halfsize.x = abs(bounding_box.Upper().Xf() - bounding_box.Lower().Xf()) / 2.0f;
        halfsize.y = abs(bounding_box.Upper().Yf() - bounding_box.Lower().Yf()) / 2.0f;

        halfsize.y -= 0.6f; //this somehow makes it look more centered

        //start centered
        offset = -halfsize;

        /// FIXME
        /*
        if((alignment & ui_align_top) > 0)
        {
            ASSERT((alignment & ui_align_bottom) == 0, "No vertical stretch for text");

            offset.y += parent_halfsize.y;
            offset.y -= halfsize.y;
        }
        else if((alignment & ui_align_bottom) > 0)
        {
            offset.y -= parent_halfsize.y;
            offset.y += halfsize.y;
        }

        if((alignment & ui_align_left) > 0)
        {
            if ((alignment & ui_align_right) > 0)
            {
                //todo: justified alignment
                ASSERT(false, "todo: justified text alignment");
            }
            else
            {
                offset.x -= parent_halfsize.x;
                offset.x += halfsize.x;
            }
        }
        else if ((alignment & ui_align_right) > 0)
        {
           offset.x += parent_halfsize.x;
           offset.x -= halfsize.x;
        }
        */
    }
}

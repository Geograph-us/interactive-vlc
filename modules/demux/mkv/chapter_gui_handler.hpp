// Copyright (C) 2024 VLC authors and VideoLAN
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// chapter_gui_handler.hpp: user interaction and display handling for chapter codecs
// Author: Khalid Masum <khalid.masum.92@gmail.com>
// Author: Steve Lhomme <steve.lhomme@free.fr>

#ifndef VLC_MKV_CHAPTER_GUI_HANDLER
#define VLC_MKV_CHAPTER_GUI_HANDLER

#include "mkv.hpp"
#include "matroska_segment.hpp"

#include <vlc_subpicture.h>

namespace mkv {
class mouse_operable {
public:
    virtual unsigned get_x_start() = 0;
    virtual unsigned get_y_start() = 0;
    virtual unsigned get_x_end() = 0;
    virtual unsigned get_y_end() = 0;

    virtual void do_on_mouse_pressed() = 0;
    virtual void do_on_mouse_over() = 0;

    virtual void update() = 0 ;

    virtual ~mouse_operable(){}
};


class mouse_event_handler {
    std::vector<mouse_operable*> operables;

    unsigned video_height, video_width;
    bool check_inside(mouse_operable *op, const unsigned x, const unsigned y) const;
    public:

    mouse_event_handler(const unsigned h, const unsigned w)
    {
        video_width = w;
        video_height = h;
    }
    void addMouseOperable(mouse_operable *op);
    void clearMouseOperables();

    bool executeOnMousePressed(const unsigned x, const unsigned y) const;
    void executeOnMouseOver(const unsigned x, const unsigned y) const;

    void update() const;
};

class choice_button;

class choice_palette
{
    bool requires_update;
    unsigned video_width;
    std::vector<choice_button*> button_list;
public:
    video_palette_t button_bg_palette;
    video_format_t bg_fmt;
    chapter_codec_vm::choices &choice_map;

    int width;
    int height;
    int selected;

    int btn_count = 0;

    subpicture_t * subpic;


    mouse_event_handler evh;

    choice_palette(subpicture_t *subpicture,
                   unsigned video_height,
                   unsigned video_width,
                   chapter_codec_vm::choices &choice
                   )
    :choice_map(choice),
     evh(video_height, video_width)
    {
        this->video_width = video_width;
        this->width = video_width/1;
        this->height = video_height/10;
        subpic = subpicture;

        button_bg_palette.palette[0][0] = 0x10; // R
        button_bg_palette.palette[0][1] = 0x10; // G
        button_bg_palette.palette[0][2] = 0x10; // B
        button_bg_palette.palette[0][3] = 0x00; // A
        button_bg_palette.i_entries = 1;

        video_format_Init(&bg_fmt, VLC_CODEC_RGBP);
        bg_fmt.i_width          =
        bg_fmt.i_visible_width  = width;
        bg_fmt.i_height         =
        bg_fmt.i_visible_height = height;
        bg_fmt.i_sar_num        = 1;
        bg_fmt.i_sar_den        = 1;
        bg_fmt.p_palette        = &button_bg_palette;
    }

    void setNumberOfButtons(unsigned n_buttons);

    void create_button (const chapter_codec_vm::choice_uid &uid, const chapter_codec_vm::chapter_choice &choice);
    void clear_buttons();

    void update();
    void try_mouse_click(const unsigned x, const unsigned y);
    void try_mouse_over(const unsigned x, const unsigned y);

    void MarkGroupChoiceSelected(const chapter_codec_vm::choice_uid &uid, const chapter_codec_vm::choice_group &group);

    bool IsChoiceSelected(const chapter_codec_vm::choice_uid &uid, const chapter_codec_vm::choice_group &group);

};

class choice_button: public mouse_operable
{
    subpicture_region_t *button_bg;
    subpicture_region_t *button_spu;
    choice_palette *palette;
    const std::string txt;
    chapter_codec_vm::chapter_choice choice;
    const chapter_codec_vm::choice_uid &uid;

    unsigned i;
    unsigned start_x, start_y, end_x, end_y;

    void update_dims(const unsigned button_width, const unsigned button_height, unsigned i);
    public:
    unsigned get_x_start() override;
    unsigned get_y_start() override;
    unsigned get_x_end() override;
    unsigned get_y_end() override;

    choice_button(choice_palette *palette,
                  const chapter_codec_vm::choice_uid &uid,
                  const chapter_codec_vm::chapter_choice &choice,
                  unsigned i)
    :txt(choice.per_language_text.cbegin()->second),
     choice(choice),
     uid(uid)
    {
        this->palette = palette;
        this->i = i;
        update_dims(palette->width, palette->height, i);
    }

    ~choice_button(){}

    void update() override;
    void do_on_mouse_pressed() override;
    void do_on_mouse_over() override;
};

}
#endif

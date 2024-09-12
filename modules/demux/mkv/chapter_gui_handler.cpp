// Copyright (C) 2024 VLC authors and VideoLAN
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// chapter_gui_handler.cpp: user interaction and display handling for chapter codecs
// Author: Khalid Masum <khalid.masum.92@gmail.com>
// Author: Steve Lhomme <steve.lhomme@free.fr>


#include "chapter_gui_handler.hpp"

namespace mkv {

bool mouse_event_handler::check_inside(mouse_operable *op, unsigned x, unsigned y) const
{
    unsigned b_x_start = op->get_x_start();
    unsigned b_y_start = video_height - op->get_y_start();
    unsigned b_x_end = op->get_x_end();
    unsigned b_y_end =  video_height - op->get_y_end();
    if ( x < b_x_start )
        return false;
    if ( x > b_x_end )
        return false;
    if ( y > b_y_start )
        return false;
    if ( y < b_y_end )
        return false;

    return true;
}

void mouse_event_handler::addMouseOperable(mouse_operable *op) {
    operables.push_back(op);
}

void mouse_event_handler::clearMouseOperables() {
    operables.clear();
}

bool mouse_event_handler::executeOnMousePressed(const unsigned x, const unsigned y) const {
    bool pressed = false;
    for ( mouse_operable *op : operables) {
        if (check_inside(op, x, y)) {
            op->do_on_mouse_pressed();
            pressed = true;
        }
    }
    return pressed;
}

void mouse_event_handler::executeOnMouseOver( const unsigned x, const unsigned y) const {
    for ( mouse_operable *op : operables) {
        if (check_inside(op, x, y))
            op->do_on_mouse_over();
    }
 }


void mouse_event_handler::update() const {
    for ( mouse_operable *op : operables) {
        op->update();
    }
}

void choice_palette::create_button (const chapter_codec_vm::choice_uid &uid, const chapter_codec_vm::chapter_choice &choice)
{
    unsigned button_no = button_list.size();
    choice_button *btn = new choice_button(this, uid, choice, button_no);
    button_list.push_back(btn);
    evh.addMouseOperable(btn);
}

void choice_palette::clear_buttons()
{
    for (auto *b : button_list)
        delete b;

    button_list.clear();
}

void choice_palette::setNumberOfButtons(unsigned n_buttons)
{
    width = video_width/n_buttons;
    bg_fmt.i_width          =
    bg_fmt.i_visible_width  = width;
}

void choice_palette::update()
{
    if (!requires_update)
        return;
    requires_update = false;
    evh.update();
}

void choice_palette::try_mouse_click(const unsigned x, const unsigned y)
{
    requires_update = evh.executeOnMousePressed(x, y);
}

void choice_palette::try_mouse_over(const unsigned x, const unsigned y)
{
    evh.executeOnMouseOver(x, y);
}

void choice_palette::MarkGroupChoiceSelected(const chapter_codec_vm::choice_uid &uid, const chapter_codec_vm::choice_group &group)
{
    choice_map.SetSelected(uid, group);
}


bool choice_palette::IsChoiceSelected(const chapter_codec_vm::choice_uid &uid, const chapter_codec_vm::choice_group &group)
{
    bool selected = choice_map.GetSelected(group) == uid;
    return selected;
}

void choice_button::update_dims(const unsigned button_width, const unsigned button_height, unsigned i)
{
    start_x = button_width*i;
    start_y = 0;
    end_x = start_x + button_width;
    end_y = button_height;
}

unsigned choice_button::get_x_start()
{
    return start_x;
}

unsigned choice_button::get_y_start()
{
    return start_y;
}

unsigned choice_button::get_x_end()
{
    return end_x;
}

unsigned choice_button::get_y_end()
{
    return end_y;
}

void choice_button::update()
{
    button_bg = subpicture_region_New(&palette->bg_fmt);
    memset(button_bg->p_picture->p[0].p_pixels, 0, button_bg->p_picture->p[0].i_pitch * button_bg->p_picture->p[0].i_visible_lines);

    button_bg->i_x = start_x;
    button_bg->i_y = start_y;
    button_bg->i_align = SUBPICTURE_ALIGN_BOTTOM | SUBPICTURE_ALIGN_LEFT;
    vlc_spu_regions_push(&palette->subpic->regions, button_bg);

    button_spu = subpicture_region_NewText();
    button_spu->p_text = text_segment_New(txt.c_str());
    button_spu->p_text->style = text_style_Create( STYLE_NO_DEFAULTS );

    button_spu->p_text->style->i_font_color = 0xFFFFFF;
    button_spu->p_text->style->i_features |= STYLE_HAS_FONT_COLOR;
    if (palette->IsChoiceSelected(uid, choice.group))
    {
        button_spu->p_text->style->i_outline_color = 0xf48b00;
        button_spu->p_text->style->i_outline_width = palette->width/ 30;
        button_spu->p_text->style->i_features |= STYLE_HAS_OUTLINE_COLOR;
    }
    button_spu->i_max_width = palette->width;
    button_spu->i_max_height = palette->height;
    button_spu->i_x = palette->width* i;
    button_spu->i_y = 0;
    button_spu->i_align = SUBPICTURE_ALIGN_BOTTOM | SUBPICTURE_ALIGN_LEFT;
    vlc_spu_regions_push(&palette->subpic->regions, button_spu);
}

void choice_button::do_on_mouse_pressed()
{
    palette->MarkGroupChoiceSelected(uid, choice.group);
}

void choice_button::do_on_mouse_over()
{
}

}

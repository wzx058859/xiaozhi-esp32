#include "esp32_s3_emoji_display.h"

#include <esp_log.h>

#include <algorithm>
#include <cstring>
#include <string>

#include "font_awesome_symbols.h"

#define TAG "Esp32S3EmojiDisplay"

// 表情映射表 - 将多种表情映射到现有6个GIF
const Esp32S3EmojiDisplay::EmotionMap Esp32S3EmojiDisplay::emotion_maps_[] = {
    // 中性/平静类表情 -> staticstate
    {"neutral", &staticstate},
    {"relaxed", &staticstate},
    {"sleepy", &staticstate},

    // 积极/开心类表情 -> happy
    {"happy", &happy},
    {"laughing", &happy},
    {"funny", &happy},
    {"loving", &happy},
    {"confident", &happy},
    {"winking", &happy},
    {"cool", &happy},
    {"delicious", &happy},
    {"kissy", &happy},
    {"silly", &happy},

    // 悲伤类表情 -> sad
    {"sad", &sad},
    {"crying", &sad},

    // 愤怒类表情 -> anger
    {"angry", &anger},

    // 惊讶类表情 -> scare
    {"surprised", &scare},
    {"shocked", &scare},

    // 思考/困惑类表情 -> buxue
    {"thinking", &buxue},
    {"confused", &buxue},
    {"embarrassed", &buxue},

    {nullptr, nullptr}  // 结束标记
};

Esp32S3EmojiDisplay::Esp32S3EmojiDisplay(esp_lcd_panel_io_handle_t panel_io,
                                          esp_lcd_panel_handle_t panel, int width, int height,
                                          int offset_x, int offset_y, bool mirror_x, bool mirror_y,
                                          bool swap_xy, DisplayFonts fonts)
    : SpiLcdDisplay(panel_io, panel, width, height, offset_x, offset_y, mirror_x, mirror_y, swap_xy,
                    fonts),
      emotion_gif_(nullptr) {
    SetupGifContainer();
}

void Esp32S3EmojiDisplay::SetupGifContainer() {
    DisplayLockGuard lock(this);

    if (emotion_label_) {
        lv_obj_del(emotion_label_);
    }
    if (chat_message_label_) {
        lv_obj_del(chat_message_label_);
    }
    if (content_) {
        lv_obj_del(content_);
    }

    content_ = lv_obj_create(container_);
    lv_obj_set_scrollbar_mode(content_, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(content_, LV_HOR_RES, LV_HOR_RES);
    lv_obj_set_style_bg_opa(content_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(content_, 0, 0);
    lv_obj_set_flex_grow(content_, 1);
    lv_obj_center(content_);

    emotion_label_ = lv_label_create(content_);
    lv_label_set_text(emotion_label_, "");
    lv_obj_set_width(emotion_label_, 0);
    lv_obj_set_style_border_width(emotion_label_, 0, 0);
    lv_obj_add_flag(emotion_label_, LV_OBJ_FLAG_HIDDEN);

    emotion_gif_ = lv_gif_create(content_);
    int gif_size = LV_HOR_RES;
    lv_obj_set_size(emotion_gif_, gif_size, gif_size);
    lv_obj_set_style_border_width(emotion_gif_, 0, 0);
    lv_obj_set_style_bg_opa(emotion_gif_, LV_OPA_TRANSP, 0);
    lv_obj_center(emotion_gif_);
    lv_gif_set_src(emotion_gif_, &staticstate);

    chat_message_label_ = lv_label_create(content_);
    lv_label_set_text(chat_message_label_, "");
    lv_obj_set_width(chat_message_label_, LV_HOR_RES * 0.9);
    lv_label_set_long_mode(chat_message_label_, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(chat_message_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(chat_message_label_, lv_color_white(), 0);
    lv_obj_set_style_border_width(chat_message_label_, 0, 0);

    lv_obj_set_style_bg_opa(chat_message_label_, LV_OPA_70, 0);
    lv_obj_set_style_bg_color(chat_message_label_, lv_color_black(), 0);
    lv_obj_set_style_pad_ver(chat_message_label_, 5, 0);

    lv_obj_align(chat_message_label_, LV_ALIGN_BOTTOM_MID, 0, 0);

    LcdDisplay::SetTheme("dark");
}

void Esp32S3EmojiDisplay::SetEmotion(const char* emotion) {
    if (!emotion || !emotion_gif_) {
        return;
    }

    DisplayLockGuard lock(this);

    for (const auto& map : emotion_maps_) {
        if (map.name && strcmp(map.name, emotion) == 0) {
            lv_gif_set_src(emotion_gif_, map.gif);
            ESP_LOGI(TAG, "设置表情: %s", emotion);
            return;
        }
    }

    lv_gif_set_src(emotion_gif_, &staticstate);
    ESP_LOGI(TAG, "未知表情'%s'，使用默认", emotion);
}

void Esp32S3EmojiDisplay::SetChatMessage(const char* role, const char* content) {
    DisplayLockGuard lock(this);
    if (chat_message_label_ == nullptr) {
        return;
    }

    if (content == nullptr || strlen(content) == 0) {
        lv_obj_add_flag(chat_message_label_, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_label_set_text(chat_message_label_, content);
    lv_obj_clear_flag(chat_message_label_, LV_OBJ_FLAG_HIDDEN);

    ESP_LOGI(TAG, "设置聊天消息 [%s]: %s", role, content);
}

void Esp32S3EmojiDisplay::SetIcon(const char* icon) {
    if (!icon) {
        return;
    }

    DisplayLockGuard lock(this);

    if (chat_message_label_ != nullptr) {
        std::string icon_message = std::string(icon) + " ";

        if (strcmp(icon, FONT_AWESOME_DOWNLOAD) == 0) {
            icon_message += "正在升级...";
        } else {
            icon_message += "系统状态";
        }

        lv_label_set_text(chat_message_label_, icon_message.c_str());
        lv_obj_clear_flag(chat_message_label_, LV_OBJ_FLAG_HIDDEN);

        ESP_LOGI(TAG, "设置图标: %s", icon);
    }
} 
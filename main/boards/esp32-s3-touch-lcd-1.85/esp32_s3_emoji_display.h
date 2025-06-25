#pragma once

#include <libs/gif/lv_gif.h>

#include "display/lcd_display.h"
#include "esp32_s3_emoji_gif.h"

/**
 * @brief ESP32-S3-Touch-LCD-1.85 GIF表情显示类
 * 继承SpiLcdDisplay，添加GIF表情支持
 */
class Esp32S3EmojiDisplay : public SpiLcdDisplay {
public:
    /**
     * @brief 构造函数，参数与SpiLcdDisplay相同
     */
    Esp32S3EmojiDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel,
                        int width, int height, int offset_x, int offset_y, bool mirror_x,
                        bool mirror_y, bool swap_xy, DisplayFonts fonts);

    virtual ~Esp32S3EmojiDisplay() = default;

    // 重写表情设置方法
    virtual void SetEmotion(const char* emotion) override;

    // 重写聊天消息设置方法
    virtual void SetChatMessage(const char* role, const char* content) override;

    // 重写图标设置方法
    virtual void SetIcon(const char* icon) override;

private:
    void SetupGifContainer();

    lv_obj_t* emotion_gif_;  ///< GIF表情组件

    // 表情映射
    struct EmotionMap {
        const char* name;
        const lv_image_dsc_t* gif;
    };

    static const EmotionMap emotion_maps_[];
}; 
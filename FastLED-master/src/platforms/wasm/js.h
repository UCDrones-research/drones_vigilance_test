#pragma once

#include "ui/ui_internal.h"
#include <memory>
#include <stdint.h>
#include <stdio.h>
#include <string>

#include <emscripten.h>
#include <emscripten/emscripten.h> // Include Emscripten headers

#include "engine_events.h"
#include "namespace.h"
#include "screenmap.h"
#include "active_strip_data.h"

FASTLED_NAMESPACE_BEGIN

// Needed or the wasm compiler will strip them out.
// Provide missing functions for WebAssembly build.
extern "C" {

// Replacement for 'millis' in WebAssembly context
EMSCRIPTEN_KEEPALIVE uint32_t millis();

// Replacement for 'micros' in WebAssembly context
EMSCRIPTEN_KEEPALIVE uint32_t micros();

// Replacement for 'delay' in WebAssembly context
EMSCRIPTEN_KEEPALIVE void delay(int ms);
}

// Sets the canvas size. This assumes one strip per row. This is
// method is pretty inflexible and is likely to change in the future.
void jsSetCanvasSize(int cledcontroler_id, int width, int height);

class jsSlider {
  public:
    jsSlider(const char *name, float value = 128.0f, float min = 0.0f, float max = 255.0f,
             float step = 1.0f);
    ~jsSlider();

    const char *name() const;
    void toJson(ArduinoJson::JsonObject& json) const;
    float value() const;
    void setValue(float value);
    operator float() const;
    operator uint8_t() const;
    operator uint16_t() const;
    operator int() const;

    template <typename T> T as() const { return static_cast<T>(mValue); }

    jsSlider& operator=(float value) { setValue(value); return *this; }
    jsSlider& operator=(int value) { setValue(static_cast<float>(value)); return *this; }

  private:
    void updateInternal(const ArduinoJson::JsonVariantConst& value);

    std::shared_ptr<jsUiInternal> mInternal;
    float mMin;
    float mMax;
    float mValue;
    float mStep;
};


class jsNumberField {
  public:
    jsNumberField(const char *name, double value, double min = 0, double max = 100);
    ~jsNumberField();

    const char *name() const;
    void toJson(ArduinoJson::JsonObject& json) const;
    double value() const;
    void setValue(double value);
    operator double() const;
    operator int() const;

    jsNumberField& operator=(double value) { setValue(value); return *this; }
    jsNumberField& operator=(int value) { setValue(static_cast<double>(value)); return *this; }
    bool operator==(double v) const { return value() == v; }
    bool operator==(int v) const { return value() == v; }
    bool operator!=(double v) const { return value() != v; }
    bool operator!=(int v) const { return value() != v; }

  private:
    void updateInternal(const ArduinoJson::JsonVariantConst& value);

    std::shared_ptr<jsUiInternal> mInternal;
    double mValue;
    double mMin;
    double mMax;
};


class jsCheckbox {
  public:
    jsCheckbox(const char *name, bool value);
    ~jsCheckbox();

    const char *name() const;
    void toJson(ArduinoJson::JsonObject& json) const;
    bool value() const;
    void setValue(bool value);
    operator bool() const;
    operator int() const;

    jsCheckbox& operator=(bool value) { setValue(value); return *this; }
    jsCheckbox& operator=(int value) { setValue(value != 0); return *this; }

  private:
    void updateInternal(const ArduinoJson::JsonVariantConst& value);

    std::shared_ptr<jsUiInternal> mInternal;
    bool mValue;
};

class jsButton {
  public:
    jsButton(const char *name);
    ~jsButton();

    const char *name() const;
    void toJson(ArduinoJson::JsonObject& json) const;
    bool isPressed() const;
    bool clicked() const {
        bool clickedHappened = mPressed && (mPressed != mPressedLast);
        return mClickedHappened;
    }
    operator bool() const { return clicked(); }

  private:
    struct Updater : EngineEvents::Listener {
        void init(jsButton *owner) {
            mOwner = owner;
            EngineEvents::addListener(this);
        }
        ~Updater() { EngineEvents::removeListener(this); }
        void onPlatformPreLoop2() override {
            mOwner->mClickedHappened = mOwner->mPressed && (mOwner->mPressed != mOwner->mPressedLast);
            mOwner->mPressedLast = mOwner->mPressed;
        }
        jsButton *mOwner = nullptr;
    };

    Updater mUpdater;

    void updateInternal(const ArduinoJson::JsonVariantConst& value);

    std::shared_ptr<jsUiInternal> mInternal;
    bool mPressed = false;
    bool mPressedLast = false;
    bool mClickedHappened = false;
};

void jsSetCanvasSize(const char* jsonString, size_t jsonSize);
void jsSetCanvasSize(int cledcontoller_id, uint16_t width, uint16_t height);
void jsSetCanvasSize(int cledcontoller_id, const ScreenMap& screenmap);
void jsOnFrame(ActiveStripData& active_strips);
void jsOnStripAdded(uintptr_t strip, uint32_t num_leds);
void updateJs(const char* jsonStr);

#define FASTLED_HAS_UI_BUTTON 1
#define FASTLED_HAS_UI_SLIDER 1
#define FASTLED_HAS_UI_CHECKBOX 1
#define FASTLED_HAS_UI_NUMBER_FIELD 1

typedef jsNumberField NumberField;
typedef jsSlider Slider;
typedef jsCheckbox Checkbox;
typedef jsButton Button;


FASTLED_NAMESPACE_END

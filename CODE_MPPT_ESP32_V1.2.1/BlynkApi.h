/**
 * @file       BlynkApi.h
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
 * @date       Jan 2015
 * @brief      High-level functions
 *
 */

#ifndef BlynkApi_h
#define BlynkApi_h

// back-compat

#ifdef BOARD_FIRMWARE_TYPE
    #define BLYNK_FIRMWARE_TYPE         BOARD_FIRMWARE_TYPE
#endif

#ifdef BOARD_FIRMWARE_VERSION
    #define BLYNK_FIRMWARE_VERSION      BOARD_FIRMWARE_VERSION
#endif

#ifdef BOARD_TEMPLATE_ID
    #define BLYNK_TEMPLATE_ID           BOARD_TEMPLATE_ID
#endif

// end of back-compat

#if !defined(BLYNK_FIRMWARE_TYPE) && defined(BLYNK_TEMPLATE_ID)
    #define BLYNK_FIRMWARE_TYPE         BLYNK_TEMPLATE_ID
#endif

#if !defined(BLYNK_FIRMWARE_VERSION)
    #define BLYNK_FIRMWARE_VERSION      "0.0.0"
#endif

#if !defined(BLYNK_TEMPLATE_ID) || !defined(BLYNK_TEMPLATE_NAME)
    #error "Please specify your BLYNK_TEMPLATE_ID and BLYNK_TEMPLATE_NAME"
#endif

#include <BlynkConfig.h>
#include <BlynkDebug.h>
#include <BlynkParam.h>
#include <BlynkTimer.h>
#include <BlynkHandlers.h>
#include <BlynkProtocolDefs.h>

#if defined(BLYNK_EXPERIMENTAL)
    #include <BlynkEveryN.h>
#endif

#if (BLYNK_TIMEOUT_MS < 1000 || BLYNK_TIMEOUT_MS > 10000)
    #error "BLYNK_TIMEOUT_MS should be 1000..10000 milliseconds"
#endif

#if (BLYNK_HEARTBEAT < 10 || BLYNK_HEARTBEAT > 3600)
    #error "BLYNK_HEARTBEAT should be 10..3600 seconds"
#endif

/**
 * Represents high-level functions of Blynk
 */
template <class Proto>
class BlynkApi
{
public:
    BlynkApi()
        : groupState(GROUP_NONE)
        , groupTs(0)
    {
    }

#ifdef DOXYGEN // These API here are only for the documentation

    /**
     * Connects to the server.
     * Blocks until connected or timeout happens.
     * May take less or more then timeout value.
     *
     * @param timeout    Connection timeout
     * @returns          True if connected to the server
     */
    bool connect(unsigned long timeout = BLYNK_TIMEOUT_MS*3);

    /**
     * Disconnects from the server.
     * It will not try to reconnect, until connect() is called
     */
    void disconnect();

    /**
     * @returns          True if connected to the server
     */
    bool connected();

    /**
     * Performs Blynk-related housekeeping
     * and processes incoming commands
     *
     * @param available  True if there is incoming data to process
     *                   Only used when user manages connection manually.
     */
    bool run(bool available = false);

#endif // DOXYGEN

    /**
     * Sends value to a Virtual Pin
     *
     * @param pin  Virtual Pin number
     * @param data Value to be sent
     */
    template <typename... Args>
    void virtualWrite(int pin, Args... values) {
        char mem[BLYNK_MAX_SENDBYTES];
        BlynkParam cmd(mem, 0, sizeof(mem));
        cmd.add("vw");
        cmd.add(pin);
        cmd.add_multi(values...);
        static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_HARDWARE, 0, cmd.getBuffer(), cmd.getLength()-1);
    }

    /**
     * Sends buffer to a Virtual Pin
     *
     * @param pin  Virtual Pin number
     * @param buff Data buffer
     * @param len  Length of data
     */
    void virtualWriteBinary(int pin, const void* buff, size_t len) {
        char mem[8];
        BlynkParam cmd(mem, 0, sizeof(mem));
        cmd.add("vw");
        cmd.add(pin);
        static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_HARDWARE, 0, cmd.getBuffer(), cmd.getLength(), buff, len);
    }

    /**
     * Sends BlynkParam to a Virtual Pin
     *
     * @param pin  Virtual Pin number
     * @param param
     */
    void virtualWrite(int pin, const BlynkParam& param) {
        virtualWriteBinary(pin, param.getBuffer(), param.getLength()-1);
    }

    void virtualWrite(int pin, const BlynkParamAllocated& param) {
        virtualWriteBinary(pin, param.getBuffer(), param.getLength()-1);
    }

    /**
     * Command grouping
     */
    void beginGroup() {
        if (GROUP_STARTED != groupState) {
            groupState = GROUP_PENDING;
            groupTs    = 0;
        }
    }

    void beginGroup(uint64_t timestamp) {
        if (GROUP_STARTED != groupState) {
            groupState = GROUP_PENDING;
            groupTs    = timestamp;
        }
    }

    void endGroup() {
        if (GROUP_STARTED == groupState) {
            char mem[4];
            BlynkParam cmd(mem, 0, sizeof(mem));
            cmd.add("e");
            static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_GROUP, 0, cmd.getBuffer(), cmd.getLength()-1);
        }
        groupState = GROUP_NONE;
    }

    /**
     * Handler helpers
     */
    void callWriteHandler(BlynkReq& req, const BlynkParam& param) {
        WidgetWriteHandler handler = GetWriteHandler(req.pin);
        if (handler && (handler != BlynkWidgetWrite)) {
            handler(req, param);
        } else {
            BlynkWidgetWriteDefault(req, param);
        }
    }

    void callReadHandler(BlynkReq& req) {
        WidgetReadHandler handler = GetReadHandler(req.pin);
        if (handler && (handler != BlynkWidgetRead)) {
            handler(req);
        } else {
            BlynkWidgetReadDefault(req);
        }
    }

    /**
     * Requests Server to re-send current values for all widgets.
     */
    void syncAll() {
        static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_HARDWARE_SYNC);
    }

    /**
     * Sends internal command
     */
    template <typename... Args>
    void sendInternal(Args... params) {
        char mem[BLYNK_MAX_SENDBYTES];
        BlynkParam cmd(mem, 0, sizeof(mem));
        cmd.add_multi(params...);
        static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_INTERNAL, 0, cmd.getBuffer(), cmd.getLength()-1);
    }

    /**
     * Requests App or Server to re-send current value of a Virtual Pin.
     * This will probably cause user-defined BLYNK_WRITE handler to be called.
     *
     * @param pin Virtual Pin number
     */
    template <typename... Args>
    void syncVirtual(Args... pins) {
        char mem[BLYNK_MAX_SENDBYTES];
        BlynkParam cmd(mem, 0, sizeof(mem));
        cmd.add("vr");
        cmd.add_multi(pins...);
        static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_HARDWARE_SYNC, 0, cmd.getBuffer(), cmd.getLength()-1);
    }

    /**
     * Sets property of a Widget
     *
     * @experimental
     *
     * @param pin      Virtual Pin number
     * @param property Property name ("label", "labels", "color", ...)
     * @param value    Property value
     */
    template <typename T, typename... Args>
    void setProperty(int pin, const T& property, Args... values) {
        char mem[BLYNK_MAX_SENDBYTES];
        BlynkParam cmd(mem, 0, sizeof(mem));
        cmd.add(pin);
        cmd.add(property);
        cmd.add_multi(values...);
        static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_PROPERTY, 0, cmd.getBuffer(), cmd.getLength()-1);
    }

    template <typename T>
    void setProperty(int pin, const T& property, const BlynkParam& param) {
        char mem[32];
        BlynkParam cmd(mem, 0, sizeof(mem));
        cmd.add(pin);
        cmd.add(property);
        static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_PROPERTY, 0, cmd.getBuffer(), cmd.getLength(), param.getBuffer(), param.getLength()-1);
    }

    template <typename T>
    void setProperty(int pin, const T& property, const BlynkParamAllocated& param) {
        char mem[32];
        BlynkParam cmd(mem, 0, sizeof(mem));
        cmd.add(pin);
        cmd.add(property);
        static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_PROPERTY, 0, cmd.getBuffer(), cmd.getLength(), param.getBuffer(), param.getLength()-1);
    }

    template <typename NAME>
    void logEvent(const NAME& event_name) {
        char mem[BLYNK_MAX_SENDBYTES];
        BlynkParam cmd(mem, 0, sizeof(mem));
        cmd.add(event_name);
        static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_EVENT_LOG, 0, cmd.getBuffer(), cmd.getLength()-1);
    }

    template <typename NAME, typename DESCR>
    void logEvent(const NAME& event_name, const DESCR& description) {
        char mem[BLYNK_MAX_SENDBYTES];
        BlynkParam cmd(mem, 0, sizeof(mem));
        cmd.add(event_name);
        cmd.add(description);
        static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_EVENT_LOG, 0, cmd.getBuffer(), cmd.getLength()-1);
    }

    template <typename NAME>
    void resolveEvent(const NAME& event_name) {
        char mem[BLYNK_MAX_SENDBYTES];
        BlynkParam cmd(mem, 0, sizeof(mem));
        cmd.add(event_name);
        static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_EVENT_CLEAR, 0, cmd.getBuffer(), cmd.getLength()-1);
    }

    template <typename NAME>
    void resolveAllEvents(const NAME& event_name) {
        char mem[BLYNK_MAX_SENDBYTES];
        BlynkParam cmd(mem, 0, sizeof(mem));
        cmd.add(event_name);
        cmd.add("all");
        static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_EVENT_CLEAR, 0, cmd.getBuffer(), cmd.getLength()-1);
    }

#if defined(BLYNK_EXPERIMENTAL)
    // Attention!
    // Every function in this section may be changed, removed or renamed.

    /**
     * Refreshes value of a widget by running
     * user-defined BLYNK_READ handler of a pin.
     *
     * @experimental
     *
     * @param pin Virtual Pin number
     */
    void refresh(int pin) {
        if (WidgetReadHandler handler = GetReadHandler(pin)) {
            BlynkReq req = { 0, BLYNK_SUCCESS, (uint8_t)pin };
            handler(req);
        }
    }

    /**
     * Delays for N milliseconds, handling server communication in background.
     *
     * @experimental
     * @warning Should be used very carefully, especially on platforms with small RAM.
     *
     * @param ms Milliseconds to wait
     */
    void delay(unsigned long ms) {
        uint16_t start = (uint16_t)micros();
        while (ms > 0) {
            static_cast<Proto*>(this)->run();
#if !defined(BLYNK_NO_YIELD)
            yield();
#endif
            if (((uint16_t)micros() - start) >= 1000) {
                ms--;
                start += 1000;
            }
        }
    }

#endif

protected:
    void processCmd(const void* buff, size_t len);
    void sendInfo();

    void sendPendingGroup() {
        if (GROUP_PENDING == groupState) {
            // Set groupState here as sendCmd is recursive
            groupState = GROUP_STARTED;
            if (groupTs) {
                char mem[24];
                BlynkParam cmd(mem, 0, sizeof(mem));
                cmd.add("t");
                cmd.add(groupTs);
                static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_GROUP, 0, cmd.getBuffer(), cmd.getLength()-1);
            } else {
                char mem[4];
                BlynkParam cmd(mem, 0, sizeof(mem));
                cmd.add("b");
                static_cast<Proto*>(this)->sendCmd(BLYNK_CMD_GROUP, 0, cmd.getBuffer(), cmd.getLength()-1);
            }
        }
    }

protected:
    enum GroupState {
        GROUP_NONE,
        GROUP_PENDING,
        GROUP_STARTED,
    } groupState;
    uint64_t groupTs;

};


#endif

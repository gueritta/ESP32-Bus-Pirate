#include "MenuTree.h"

std::vector<MenuCommand> MenuTree::getGeneralCommands() {
    return {
        {"help", "Show help", false},
        {"mode", "Select mode", true},
        {"system", "Show system info", false},
        {"logic", "Logic analyzer", false},
        {"analogic", "Analogic plotter", false},
        {"wizard", "Pin diagnostic", false},
        {"hex", "Hexadecimal converter", true},
        {"profile", "Load/save pin profiles", false},
        {"listen", "Pin activity to audio", true},
        {"delay", "Delay ms", true},
        {"repeat", "Repeat command", true}
    };
}

std::vector<MenuCommand> MenuTree::getProtocolCommands(ModeEnum mode) {
    switch (mode) {
        case ModeEnum::OneWire:
            return {
                {"scan", "Scan 1-Wire bus", false},
                {"ping", "Ping 1-Wire device", false},
                {"sniff", "Sniff 1-Wire bus", false},
                {"read", "Read from device", false},
                {"write", "Write to device", true},
                {"temp", "Read DS18B20", false},
                {"ibutton", "iButton operations", true},
                {"eeprom", "EEPROM operations", true},
                {"config", "Configure settings", false}
            };
        case ModeEnum::UART:
        case ModeEnum::HDUART:
            return {
                {"autobaud", "Detect baud rate", false},
                {"bridge", "UART bridge", false},
                {"at", "Send AT command", true},
                {"spam", "Spam UART", true},
                {"glitch", "UART glitch", false},
                {"xmodem", "XModem transfer", true},
                {"swap", "Swap RX/TX", false},
                {"emulator", "UART emulator", false},
                {"trigger", "UART trigger", true},
                {"raw", "Raw UART mode", false}
            };
        case ModeEnum::I2C:
            return {
                {"discovery", "Discover devices", false},
                {"identify", "Identify device", false},
                {"slave", "I2C slave mode", false},
                {"dump", "Dump memory", true},
                {"flood", "Flood I2C bus", false},
                {"health", "Health check", false},
                {"monitor", "Monitor I2C bus", false},
                {"recover", "Recover I2C bus", false},
                {"jam", "Jam I2C bus", false},
                {"regs", "Read registers", true}
            };
        case ModeEnum::SPI:
            return {
                {"sdcard", "SD Card operations", true},
                {"flash", "Flash memory operations", true}
            };
        case ModeEnum::TwoWire:
        case ModeEnum::ThreeWire:
            return {
                {"smartcard", "Smartcard ops", true}
            };
        case ModeEnum::DIO:
            return {
                {"set", "Set pin state", true},
                {"pullup", "Set pullup", true},
                {"pulldown", "Set pulldown", true},
                {"pulse", "Generate pulse", true},
                {"servo", "Servo control", true},
                {"pwm", "PWM control", true},
                {"toggle", "Toggle pin", true},
                {"measure", "Measure frequency", true},
                {"reset", "Reset pins", false},
                {"pins", "List pins", false}
            };
        case ModeEnum::LED:
            return {
                {"fill", "Fill color", true},
                {"blink", "Blink LED", true},
                {"rainbow", "Rainbow effect", false},
                {"chase", "Chase effect", false},
                {"cycle", "Color cycle", false},
                {"wave", "Wave effect", false},
                {"setprotocol", "Set LED protocol", true}
            };
        case ModeEnum::Infrared:
            return {
                {"send", "Send IR signal", true},
                {"receive", "Receive IR signal", false},
                {"devicebgone", "OFF devices blast", false},
                {"remote", "Universal remote", true},
                {"replay", "Replay recorded", true},
                {"record", "Record IR", false},
                {"load", "Load .ir file", true}
            };
        case ModeEnum::USB:
            return {
                {"stick", "Mount SD as USB", false},
                {"keyboard", "Keyboard typing", true},
                {"mouse", "Mouse operations", true},
                {"gamepad", "Gamepad press", true},
                {"jiggle", "Mouse jiggle", true},
                {"host", "USB host mode", false},
                {"sysctrl", "System control", true}
            };
        case ModeEnum::Bluetooth:
            return {
                {"pair", "Pair device", true},
                {"spoof", "Spoof MAC", true},
                {"status", "Show status", false},
                {"server", "BT HID server", false}
            };
        case ModeEnum::WiFi:
            return {
                {"connect", "Connect to Wi-Fi", true},
                {"probe", "Search access", false},
                {"deauth", "Deauth host", true},
                {"disconnect", "Disconnect Wi-Fi", false},
                {"ap", "Access point", true},
                {"ssh", "SSH session", true},
                {"telnet", "Telnet session", true},
                {"nc", "Netcat session", true},
                {"nmap", "Scan ports", true},
                {"modbus", "Modbus TCP", true},
                {"repeater", "Wi-Fi repeater", false},
                {"extender", "Wi-Fi extender", false},
                {"http", "HTTP request", true},
                {"lookup", "Lookup IP/MAC", true},
                {"webui", "Show Web UI", false},
                {"flood", "Flood channel", true}
            };
        case ModeEnum::JTAG:
            return {
                {"scan swd", "Scan SWD", false},
                {"scan jtag", "Scan JTAG", false}
            };
        case ModeEnum::I2S:
            return {
                {"play", "Play sine wave", true},
                {"test", "Test audio", true}
            };
        case ModeEnum::SUBGHZ:
            return {
                {"sweep", "Analyze band", false},
                {"decode", "Decode signal", false},
                {"bruteforce", "Bruteforce keys", false},
                {"trace", "Trace signal", false},
                {"setfrequency", "Set freq", true},
                {"waterfall", "Waterfall", false},
                {"ear", "Audio mapping", false}
            };
        case ModeEnum::RFID:
            return {
                {"clone", "Clone tag", false},
                {"erase", "Erase tag", false}
            };
        case ModeEnum::RF24_:
            return {
                {"setchannel", "Set channel", true}
            };
        case ModeEnum::FM:
            return {
                {"broadcast", "FM broadcast", true}
            };
        case ModeEnum::CELL:
            return {
                {"call", "Call ops", true},
                {"sms", "SMS ops", true},
                {"modem", "Modem ops", true},
                {"sim", "SIM ops", true},
                {"network", "Network ops", false},
                {"unlock", "Unlock SIM", true},
                {"ussd", "USSD code", true},
                {"setmode", "Set mode", true},
                {"operator", "Operator ops", false},
                {"phonebook", "Phonebook ops", false}
            };
        default:
            return {};
    }
}

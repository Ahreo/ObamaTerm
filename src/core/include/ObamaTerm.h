#ifndef OBAMA_TERM
#define OBAMA_TERM

#include "olcPixelGameEngine.h"
#include "olcPGEX_QuickGUI.h"

#include "SerialPort.h"
#include "PortManager.h"

namespace OT {
class ObamaTerm : public olc::PixelGameEngine {
public:
    ObamaTerm() {
        sAppName = "Obama Term";
    }

public:
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
    
protected:
    olc::QuickGUI::Manager guiManager;

    olc::QuickGUI::Button* guiInitConfigButton = nullptr;
    olc::QuickGUI::Button* guiCancelButton = nullptr;

    olc::QuickGUI::ListBox* guiListPort = nullptr;
    std::vector<std::string> portList;
    
    olc::QuickGUI::ListBox* guiListBaud = nullptr;
    std::vector<std::string> baudList;

    olc::QuickGUI::Slider* guiScrollBar = nullptr;

private:
    OT::PortManager* manager = nullptr;
    OT::SerialPort* serial = nullptr;
    bool m_start_polling = false;
    uint32_t m_curr_pos = 0;
};
}

#endif // OBAMA_TERM

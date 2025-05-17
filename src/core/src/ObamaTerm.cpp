#include "ObamaTerm.h"

namespace OT {
    bool ObamaTerm::OnUserCreate() {
        // PortManager singleton
        manager = OT::PortManager::get_instance();

        // Init portname vector
        portList = manager->get_portname_vec();

        // Port List
        guiListPort = new olc::QuickGUI::ListBox(guiManager,
			portList, { 150.0f, 130.0f }, { 100.0f, 40.0f });

        // Baud list
        baudList.push_back("9600");
        baudList.push_back("115200");
        guiListBaud = new olc::QuickGUI::ListBox(guiManager,
			baudList, { 150.0f, 180.0f }, { 100.0f, 40.0f });

        // Slider to be used as a scroll bar
        guiScrollBar = new olc::QuickGUI::Slider(guiManager, 
            {(float) ScreenWidth(), 0}, {(float) ScreenWidth(), (float) ScreenHeight()}, 
            0, 100, 0);

		// TextBox, allows basic text entry
        guiInitConfigButton = new olc::QuickGUI::Button(guiManager,
            "Ok", { 30.0f, 150.0f }, { 100.0f, 16.0f });
        guiCancelButton = new olc::QuickGUI::Button(guiManager,
            "Cancel", { 30.0f, 170.0f }, { 100.0f, 16.0f });
		return true;
    }

    bool ObamaTerm::OnUserUpdate(float fElapsedTime)  {
		// We must update the manager at some point each frame. Values of controls
		// are only valid AFTER this call to update()
		guiManager.Update(this);
		// Clear(olc::BLACK);

        if(guiInitConfigButton->bPressed) {
            std::vector<std::string> port_names = manager->get_portname_vec();
            serial = manager->get_serial_port(port_names[guiListPort->nSelectedItem]);
            m_start_polling = true;
        }

        if(m_start_polling) {
            std::vector<uint8_t> buf = serial->receive_data();
            if(!buf.empty()) {
                for(uint8_t byte : buf) {
                    // DrawString(m_curr_pos, 30, (char*) buf.data(), olc::WHITE, 1);
                    // m_curr_pos += GetPixelSize().x;
                    printf("%c IN ", byte);
                }
            }
        }

        guiManager.DrawDecal(this);
		return true;
	}
}

#include "ObamaTerm.h"

namespace OT {
    bool ObamaTerm::OnUserCreate() {
        OT::PortManager* manager = OT::PortManager::get_instance();
        OT::SerialPort* serial = manager->get_port("COM5");
        printf("Name:\n %s \n", serial->get_name().c_str());
        printf("Desciption:\n %s \n", serial->get_desc().c_str());
        printf("Transport:\n %s \n", serial->get_trans_protocol().c_str());

        portList = manager->get_portname_vec();

        // Port List
        guiListPort = new olc::QuickGUI::ListBox(guiManager,
			portList, { 150.0f, 130.0f }, { 100.0f, 20.0f });

        // Baud list
        baudList.push_back("9600");
        baudList.push_back("115200");
        guiListBaud = new olc::QuickGUI::ListBox(guiManager,
			baudList, { 150.0f, 180.0f }, { 100.0f, 20.0f });

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

		Clear(olc::BLACK);

        guiManager.Draw(this);
		return true;
	}
}

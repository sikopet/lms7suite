/**
@file 	myriad7_wxgui.cpp
@author Lime Microsystems
@brief	panel for controlling Myriad7 board GPIO
*/
#include "myriad7_wxgui.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/combobox.h>
#include <wx/msgdlg.h>

#include "lmsComms.h"

const long Myriad7_wxgui::ID_STATICTEXT1 = wxNewId();
const long Myriad7_wxgui::ID_COMBOBOX1 = wxNewId();
const long Myriad7_wxgui::ID_STATICTEXT2 = wxNewId();
const long Myriad7_wxgui::ID_COMBOBOX2 = wxNewId();

BEGIN_EVENT_TABLE(Myriad7_wxgui,wxFrame)
END_EVENT_TABLE()

Myriad7_wxgui::Myriad7_wxgui(wxWindow* parent,wxWindowID id, const wxString &title, const wxPoint& pos,const wxSize& size, long styles)
{
	wxFlexGridSizer* FlexGridSizer2;
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, id, title, wxDefaultPosition, wxDefaultSize, styles, title);
#ifdef WIN32
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
#endif
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	FlexGridSizer2 = new wxFlexGridSizer(0, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _T("Rx (A/B) Input:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer2->Add(StaticText1, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	cmbGPIO_1_0 = new wxComboBox(this, ID_COMBOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX1"));
	cmbGPIO_1_0->SetSelection( cmbGPIO_1_0->Append(_T("No connection")) );
	cmbGPIO_1_0->Append(_T("LNAW"));
	cmbGPIO_1_0->Append(_T("LNAH"));
	cmbGPIO_1_0->Append(_T("LNAL"));
	FlexGridSizer2->Add(cmbGPIO_1_0, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _T("Tx (A/B) output:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer2->Add(StaticText2, 1, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	cmbGPIO2 = new wxComboBox(this, ID_COMBOBOX2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX2"));
	cmbGPIO2->SetSelection( cmbGPIO2->Append(_T("TX1")) );
	cmbGPIO2->Append(_T("TX2"));
	FlexGridSizer2->Add(cmbGPIO2, 1, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(ID_COMBOBOX1,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&Myriad7_wxgui::ParameterChangeHandler);
	Connect(ID_COMBOBOX2,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&Myriad7_wxgui::ParameterChangeHandler);
}

Myriad7_wxgui::~Myriad7_wxgui()
{	
}

void Myriad7_wxgui::Initialize(LMScomms* pSerPort)
{
    serPort = pSerPort;
    assert(serPort != nullptr);
}

void Myriad7_wxgui::ParameterChangeHandler(wxCommandEvent& event)
{   
    assert(serPort != nullptr);
    if (serPort == nullptr)
        return;
    unsigned rxInput = cmbGPIO_1_0->GetSelection();
    unsigned txOutput = cmbGPIO2->GetSelection();
    unsigned char value = txOutput << 2 | rxInput;
    LMScomms::GenericPacket pkt;
    pkt.cmd = CMD_GPIO_WR;
    pkt.outBuffer.push_back(value);
    if (serPort->TransferPacket(pkt) != LMScomms::TRANSFER_SUCCESS)
    {
        wxMessageBox(_("Failed to write Myriad7 GPIOs"), _("Error"), wxICON_ERROR | wxOK);
        return;
    }
    if (pkt.status != STATUS_COMPLETED_CMD)
        wxMessageBox(_("Board response: ") + wxString::From8BitData(status2string(pkt.status)), _("Warning"), wxICON_WARNING | wxOK);
}

void Myriad7_wxgui::UpdatePanel()
{   
    assert(serPort != nullptr);
    if (serPort == nullptr || serPort->GetInfo().expansion != EXP_BOARD_MYRIAD7)
        return;
    
    LMScomms::GenericPacket pkt;
    pkt.cmd = CMD_GPIO_RD;
    if (serPort->TransferPacket(pkt) != LMScomms::TRANSFER_SUCCESS)
    {
        wxMessageBox(_("Failed to read Myriad7 GPIOs"), _("Error"), wxICON_ERROR | wxOK);
        return;
    }
    if (pkt.status != STATUS_COMPLETED_CMD)
    {
        wxMessageBox(_("Board response: ") + wxString::From8BitData(status2string(pkt.status)), _("Warning"), wxICON_WARNING | wxOK);
        return;
    }
    cmbGPIO_1_0->SetSelection(pkt.inBuffer[0] & 0x3);
    cmbGPIO2->SetSelection((pkt.inBuffer[0] & 0x4) > 0);
}

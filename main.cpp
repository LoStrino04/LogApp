#include <wx/wx.h>
#include <wx/grid.h>
#include "mathplot.h"
#include "readlog.h"


class AppFrame : public wxFrame {
public:
    AppFrame(const wxString& titolo, const wxPoint& pos, const wxSize& size);

private:
    mpWindow* first_plot;
    mpWindow* second_plot;
    wxListBox* ChannelList;
    wxListBox* IdList;
    wxListBox* DbcList;
    wxMenuBar* menuBar;
    wxMenu* file_menu;
    wxMenuItem* export_csv_1;
    wxMenuItem* open_file;
    wxMenuItem* export_csv_2;
    wxMenu* options_menu;
    wxMenu* help_menu;
    wxMenuItem* quit;
    wxButton* first_nextDataButton;
    wxButton* first_previousDataButton;
    wxButton* second_nextDataButton;
    wxButton* second_previousDataButton;
    wxButton* plot_one;
    wxButton* plot_two;
    wxButton* insert_dbc;
    wxButton* delete_dbc;
    wxTextCtrl* text_dbc;
    wxStaticText* ch_text;
    wxStaticText* id_text;
    wxStaticText* dbc_text;
    vector<Log> logs_from_blf;
    vector<vector<vector<Log>>> log_channels;

    // variabili per visualizzazione grafici
    int first_selected_channel = 0, first_selected_id = 0, first_visual_limit = BASE_VISUAL_LIMIT;
    int second_selected_channel = 0, second_selected_id = 0, second_visual_limit = BASE_VISUAL_LIMIT;
    int tmp_selected_plot = 0;

    void CreatePlot(int channel, int index, int visual_limit, mpWindow* in_plot);
    void OnChannelListSelect(wxCommandEvent& event);
    void OnIdListSelect(wxCommandEvent& event);
    void OnFirstNextData(wxCommandEvent& event);
    void OnFirstPreviousData(wxCommandEvent& event);
    void OnSecondNextData(wxCommandEvent& event);
    void OnSecondPreviousData(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnExport1(wxCommandEvent& event);
    void OnExport2(wxCommandEvent& event);
    void OnPlotOne(wxCommandEvent& event);
    void OnPlotTwo(wxCommandEvent& event);
    void OnInsertDBC(wxCommandEvent& event);
    void OnDeleteDBC(wxCommandEvent& event);
};

AppFrame::AppFrame(const wxString& titolo, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, wxID_ANY, titolo, pos, size) {

    wxPanel* panel = new wxPanel(this, wxID_ANY);

    //creazione barra dei menu
    menuBar = new wxMenuBar();
    file_menu = new wxMenu();
    options_menu = new wxMenu();
    help_menu = new wxMenu();
    open_file = new wxMenuItem(file_menu, 0, "Open BLF");
    export_csv_1 = new wxMenuItem(file_menu, 1, "Export CSV Upper Plot");
    export_csv_2 = new wxMenuItem(file_menu, 2, "Export CSV Lower Plot");

    file_menu->Append(open_file);
    file_menu->Append(export_csv_1);
    file_menu->Append(export_csv_2);
    file_menu->Bind(wxEVT_MENU, &AppFrame::OnOpen, this, open_file->GetId());
    file_menu->Bind(wxEVT_MENU, &AppFrame::OnExport1, this, export_csv_1->GetId());
    file_menu->Bind(wxEVT_MENU, &AppFrame::OnExport2, this, export_csv_2->GetId());
    menuBar->Append(file_menu, "File");
    menuBar->Append(options_menu, "Plot");
    menuBar->Append(help_menu, "Help");


    // Creazione della listbox con 3 opzioni
    wxString channels[] = { "CAN 0", "CAN 1" };
    ChannelList = new wxListBox(panel, wxID_ANY, wxDefaultPosition, wxSize(150, -1), 2, channels, wxLB_SINGLE);
    ChannelList->Bind(wxEVT_LISTBOX, &AppFrame::OnChannelListSelect, this);

    wxString default_ids[] = { "" };
    IdList = new wxListBox(panel, wxID_ANY, wxDefaultPosition, wxSize(150, -1), 1, default_ids, wxLB_SINGLE);
    IdList->Bind(wxEVT_LISTBOX, &AppFrame::OnIdListSelect, this);

    wxString default_dbc[] = { "" };
    DbcList = new wxListBox(panel, wxID_ANY, wxDefaultPosition, wxSize(150, -1), 0, default_ids, wxLB_SINGLE);

    //impostazione dei pulsanti per spostarsi nel grafico superiore
    first_nextDataButton = new wxButton(panel, wxID_ANY, " > ", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "nextDataButton");
    first_nextDataButton->Bind(wxEVT_BUTTON, &AppFrame::OnFirstNextData, this);
    first_previousDataButton = new wxButton(panel, wxID_ANY, " < ", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "previousDataButton");
    first_previousDataButton->Bind(wxEVT_BUTTON, &AppFrame::OnFirstPreviousData, this);

    //impostazione dei pulsanti per spostarsi nel grafico inferiore
    second_nextDataButton = new wxButton(panel, wxID_ANY, " > ", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "nextDataButton");
    second_nextDataButton->Bind(wxEVT_BUTTON, &AppFrame::OnSecondNextData, this);
    second_previousDataButton = new wxButton(panel, wxID_ANY, " < ", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "previousDataButton");
    second_previousDataButton->Bind(wxEVT_BUTTON, &AppFrame::OnSecondPreviousData, this);

    //impostazione dei pulsanti per selezionare il grafico da cambiare
    plot_one = new wxButton(panel, wxID_ANY, "Set Upper Plot", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "plot_one");
    plot_one->Bind(wxEVT_BUTTON, &AppFrame::OnPlotOne, this);
    plot_two = new wxButton(panel, wxID_ANY, "Set Lower Plot", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "plot_two");
    plot_two->Bind(wxEVT_BUTTON, &AppFrame::OnPlotTwo, this);

    //impostazione pulsanti per gestione dbc
    insert_dbc = new wxButton(panel, wxID_ANY, "Add DBC", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "insert_dbc");
    insert_dbc->Bind(wxEVT_BUTTON, &AppFrame::OnInsertDBC, this);
    delete_dbc = new wxButton(panel, wxID_ANY, "Delete DBC", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "delete_dbc");
    delete_dbc->Bind(wxEVT_BUTTON, &AppFrame::OnDeleteDBC, this);

    text_dbc = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "text_dbc");
    text_dbc->SetHint("Write DBC Path Here");

    //Impostazione testi id e channel
    ch_text = new wxStaticText(panel, wxID_ANY, "Channels", wxDefaultPosition, wxDefaultSize, 0, "ch_text");
    id_text = new wxStaticText(panel, wxID_ANY, "ID", wxDefaultPosition, wxDefaultSize, 0, "id_text");
    dbc_text = new wxStaticText(panel, wxID_ANY, "DBC", wxDefaultPosition, wxDefaultSize, 0, "dbc_text");
    wxFont tmp_font = ch_text->GetFont();
    tmp_font.SetWeight(wxFONTWEIGHT_BOLD);
    ch_text->SetFont(tmp_font);
    id_text->SetFont(tmp_font);
    dbc_text->SetFont(tmp_font);

    // Creazione del grafico
    first_plot = new mpWindow(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
    second_plot = new mpWindow(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);


    // Layout principale
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Layout liste
    wxBoxSizer* listSizer = new wxBoxSizer(wxVERTICAL);
    listSizer->Add(ch_text, 0, wxEXPAND | wxALL);
    listSizer->Add(ChannelList, 1, wxEXPAND | wxALL);
    listSizer->Add(id_text, 0, wxEXPAND | wxALL);
    listSizer->Add(IdList, 5, wxEXPAND | wxALL);
    listSizer->Add(dbc_text, 0, wxEXPAND | wxALL);
    listSizer->Add(DbcList, 5, wxEXPAND | wxALL);
    listSizer->Add(delete_dbc, 0, wxEXPAND | wxALL);

    // Layout pulsanti
    wxBoxSizer* selection_buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    selection_buttonSizer->Add(plot_one, 1, wxEXPAND | wxALL, 0);
    selection_buttonSizer->Add(plot_two, 1, wxEXPAND | wxALL, 0);
    wxBoxSizer* first_buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    first_buttonSizer->Add(first_previousDataButton, 1, wxEXPAND | wxALL, 0);
    first_buttonSizer->Add(first_nextDataButton, 1, wxEXPAND | wxALL, 0);
    wxBoxSizer* second_buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    second_buttonSizer->Add(second_previousDataButton, 1, wxEXPAND | wxALL, 0);
    second_buttonSizer->Add(second_nextDataButton, 1, wxEXPAND | wxALL, 0);
    wxBoxSizer* dbc_sizer = new wxBoxSizer(wxHORIZONTAL);
    dbc_sizer->Add(insert_dbc, 1, wxEXPAND | wxALL, 0);
    dbc_sizer->Add(text_dbc, 5, wxEXPAND | wxALL, 0);

    // Layout per grafici e pulsanti
    wxBoxSizer* button_plotSizer = new wxBoxSizer(wxVERTICAL);
    button_plotSizer->Add(dbc_sizer, 1, wxEXPAND | wxALL, 0);
    button_plotSizer->Add(selection_buttonSizer, 1, wxEXPAND | wxALL, 0);
    button_plotSizer->Add(first_plot, 12, wxEXPAND | wxALL, 0);
    button_plotSizer->Add(first_buttonSizer, 1, wxEXPAND | wxALL, 0);
    button_plotSizer->Add(second_plot, 12, wxEXPAND | wxALL, 0);
    button_plotSizer->Add(second_buttonSizer, 1, wxEXPAND | wxALL, 0);
    wxBoxSizer* list_plotSizer = new wxBoxSizer(wxHORIZONTAL);
    list_plotSizer->Add(listSizer, 0, wxEXPAND | wxALL, 10);
    list_plotSizer->Add(button_plotSizer, 1, wxEXPAND | wxALL, 10);

    // Aggiunta layout pagina al layout principale
    mainSizer->Add(list_plotSizer, 1, wxEXPAND);

    // Impostazione il layout del pannello
    panel->SetSizer(mainSizer);

    write_dbc_list(DbcList);
    SetMenuBar(menuBar);
    CreateStatusBar(4);
    Centre();
}

void AppFrame::OnOpen(wxCommandEvent& event) {
    // Legge i dati
    read_data_from_txt(logs_from_blf);
    assign_name_to_id(logs_from_blf);
    //divisione degli oggetti Log per id e per channel
    vector<vector<Log>> id_channel_zero, id_channel_one;
    list_foreach_id(logs_from_blf, id_channel_zero, 0);
    list_foreach_id(logs_from_blf, id_channel_one, 1);

    log_channels.push_back(id_channel_zero);
    log_channels.push_back(id_channel_one);
    
}

void AppFrame::OnExport1(wxCommandEvent& event) {
    write_csv_file(log_channels[first_selected_channel][first_selected_id]);
}

void AppFrame::OnExport2(wxCommandEvent& event) {
    write_csv_file(log_channels[second_selected_channel][second_selected_id]);
}

void AppFrame::OnPlotOne(wxCommandEvent& event) {
    tmp_selected_plot = 0;
    plot_one->SetBackgroundColour(*wxLIGHT_GREY);
    plot_two->SetBackgroundColour(*wxWHITE);
}

void AppFrame::OnPlotTwo(wxCommandEvent& event) {
    tmp_selected_plot = 1;
    plot_one->SetBackgroundColour(*wxWHITE);
    plot_two->SetBackgroundColour(*wxLIGHT_GREY);
}

void AppFrame::CreatePlot(int channel, int index, int visual_limit, mpWindow* in_plot) {
    
    in_plot->DelAllLayers(true); // Pulisce il grafico

    vector<double> data;
    vector<double> time_stmp;

    extract_time_data(log_channels[channel][index], time_stmp, data, visual_limit);


    // Usa la serie selezionata per il grafico
    mpFXYVector* data_layer = new mpFXYVector("", mpALIGN_CENTER);
    data_layer->SetData(time_stmp, data);
    data_layer->SetContinuity(true);

    if (in_plot == first_plot) data_layer->SetPen(wxPen(*wxRED, 2));
    else data_layer->SetPen(wxPen(*wxBLUE, 2));

    // Asse X e Y
    mpScaleX* xaxis = new mpScaleX("Time", mpALIGN_BORDER_BOTTOM, true);
    mpScaleY* yaxis = new mpScaleY("Data", mpALIGN_BORDER_LEFT, true);

    in_plot->SetMargins(20, 20, 20, 20);

    in_plot->AddLayer(xaxis);
    in_plot->AddLayer(yaxis);
    in_plot->AddLayer(data_layer);

    in_plot->EnableMousePanZoom(false);
    in_plot->Fit();
    in_plot->Refresh();

}

void AppFrame::OnChannelListSelect(wxCommandEvent& event) {
    int tmp_channel = ChannelList->GetSelection();
    if (tmp_channel != wxNOT_FOUND && !logs_from_blf.empty()) {

        if (tmp_selected_plot == 0) {
            first_selected_channel = tmp_channel;
            first_selected_id = 0;
        }
        else {
            second_selected_channel = tmp_channel;
            second_selected_id = 0;
        }

        IdList->Clear();
        vector<vector<Log>> tmp_id_vector = log_channels[tmp_channel];

        // Aggiorno la listbox degli id con quelli del channel selezionato
        for (int i = 0; i < tmp_id_vector.size(); i++) {
            wxString tmp_id;
            if (tmp_id_vector[i][0].get_name() == "")
                tmp_id << "ID: " << tmp_id_vector[i][0].get_id();
            else
                tmp_id << tmp_id_vector[i][0].get_name();

            IdList->AppendString(tmp_id);
        }
    }
}

void AppFrame::OnIdListSelect(wxCommandEvent& event) {
    int tmp_id = IdList->GetSelection();
    if (tmp_id != wxNOT_FOUND) {

        if (tmp_selected_plot == 0) {
            first_selected_id = tmp_id;
            first_visual_limit = BASE_VISUAL_LIMIT;
            CreatePlot(first_selected_channel, first_selected_id, first_visual_limit, first_plot);
        }
        else {
            second_selected_id = tmp_id;
            second_visual_limit = BASE_VISUAL_LIMIT;
            CreatePlot(second_selected_channel, second_selected_id, second_visual_limit, second_plot);
        }
    }
}

void AppFrame::OnFirstNextData(wxCommandEvent& event) {
    if (logs_from_blf.empty())
        return;
    if (first_visual_limit > log_channels[first_selected_channel][first_selected_id].size())
        return;

    first_visual_limit += BASE_VISUAL_LIMIT;
    CreatePlot(first_selected_channel, first_selected_id, first_visual_limit, first_plot);
}

void AppFrame::OnFirstPreviousData(wxCommandEvent& event) {
    if (logs_from_blf.empty())
        return;
    if (first_visual_limit <= BASE_VISUAL_LIMIT)
        return;

    first_visual_limit -= BASE_VISUAL_LIMIT;
    CreatePlot(first_selected_channel, first_selected_id, first_visual_limit, first_plot);
}

void AppFrame::OnSecondNextData(wxCommandEvent& event) {
    if (logs_from_blf.empty())
        return;
    if (second_visual_limit > log_channels[second_selected_channel][second_selected_id].size())
        return;

    second_visual_limit += BASE_VISUAL_LIMIT;
    CreatePlot(second_selected_channel, second_selected_id, second_visual_limit, second_plot);
}

void AppFrame::OnSecondPreviousData(wxCommandEvent& event) {
    if (logs_from_blf.empty())
        return;
    if (second_visual_limit <= BASE_VISUAL_LIMIT)
        return;

    second_visual_limit -= BASE_VISUAL_LIMIT;
    CreatePlot(second_selected_channel, second_selected_id, second_visual_limit, second_plot);
}

void AppFrame::OnInsertDBC(wxCommandEvent& event) {
    wxString dbc_path = text_dbc->GetValue();

    for (int i = 0; i < DbcList->GetCount(); i++) {
        wxString tmp_str = DbcList->GetString(i);

        if (tmp_str == dbc_path)
            return;
    }


    DbcList->AppendString(dbc_path);
    write_dbc_path(DbcList);

    wxString wx_dbc_name;
    wx_dbc_name << getFileName(string(dbc_path));

    DbcList->Delete(DbcList->GetCount() - 1);
    DbcList->AppendString(wx_dbc_name);

    text_dbc->Clear();
}

void AppFrame::OnDeleteDBC(wxCommandEvent& event) {
    int tmp_dbc = DbcList->GetSelection();

    if (tmp_dbc != wxNOT_FOUND) {
        DbcList->Delete(tmp_dbc);
        write_dbc_path(DbcList);
    }
}

class LogApp : public wxApp {
public:
    virtual bool OnInit();
};

bool LogApp::OnInit() {
    AppFrame* frame = new AppFrame("LogApp", wxPoint(50, 50), wxSize(1280, 720));
    frame->Show(true);
    return true;
}

wxIMPLEMENT_APP(LogApp);

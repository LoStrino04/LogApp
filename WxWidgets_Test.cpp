#include <wx/wx.h>
#include <wx/grid.h>
#include "mathplot.h"
#include "readlog.h"


class AppFrame : public wxFrame {
public:
    AppFrame(const wxString& titolo, const wxPoint& pos, const wxSize& size);

private:
    mpWindow* plot;
    wxListBox* ChannelList;
    wxListBox* IdList;
    wxGrid* dataTable;
    wxMenuBar* menuBar;
    wxMenu* file_menu;
    wxMenuItem* open_file;
    wxMenuItem* export_csv;
    wxMenu* options_menu;
    wxMenu* help_menu;
    wxMenuItem* quit;
    wxButton* nextDataButton;
    wxButton* previousDataButton;
    wxStaticText* ch_text;
    wxStaticText* id_text;
    vector<Log> logs_from_blf;
    vector<vector<vector<Log>>> log_channels;

    int visual_limit = BASE_VISUAL_LIMIT;
    int selected_channel = 0;
    int selected_id = 0;
    int num_table_cols = 4;

    void CreatePlot(int channel, int index);
    void OnChannelListSelect(wxCommandEvent& event);
    void OnIdListSelect(wxCommandEvent& event);
    void OnNextData(wxCommandEvent& event);
    void OnPreviousData(wxCommandEvent& event);
    void UpdateDataTable(vector<Log>& logs);
    void OnOpen(wxCommandEvent& event);
    void OnExport(wxCommandEvent& event);
};

AppFrame::AppFrame(const wxString& titolo, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, wxID_ANY, titolo, pos, size) {

    wxPanel* panel = new wxPanel(this, wxID_ANY);

    //creazione barra dei menu
    menuBar = new wxMenuBar();
    file_menu = new wxMenu();
    open_file = new wxMenuItem(file_menu, 0, "Open BLF");
    export_csv = new wxMenuItem(file_menu, 1, "Export CSV");
    options_menu = new wxMenu();
    help_menu = new wxMenu();

    menuBar->Append(file_menu, "File");
    menuBar->Append(options_menu, "View");
    menuBar->Append(help_menu, "Help");
    file_menu->Append(open_file);
    file_menu->Append(export_csv);
    file_menu->Bind(wxEVT_MENU, &AppFrame::OnOpen, this, open_file->GetId());
    file_menu->Bind(wxEVT_MENU, &AppFrame::OnExport, this, export_csv->GetId());
    

    // Creazione della listbox con 3 opzioni
    wxString channels[] = { "Channel 0", "Channel 1" };
    ChannelList = new wxListBox(panel, wxID_ANY, wxDefaultPosition, wxSize(150, -1), 2, channels, wxLB_SINGLE);
    ChannelList->Bind(wxEVT_LISTBOX, &AppFrame::OnChannelListSelect, this);

    wxString default_ids[] = { "" };
    IdList = new wxListBox(panel, wxID_ANY, wxDefaultPosition, wxSize(150, -1), 1, default_ids, wxLB_SINGLE);
    IdList->Bind(wxEVT_LISTBOX, &AppFrame::OnIdListSelect, this);

    //impostazione dei pulsanti per spostarsi nel grafico
    nextDataButton = new wxButton(panel, wxID_ANY, " > ", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "nextDataButton");
    nextDataButton->Bind(wxEVT_BUTTON, &AppFrame::OnNextData, this);
    previousDataButton = new wxButton(panel, wxID_ANY, " < ", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "previousDataButton");
    previousDataButton->Bind(wxEVT_BUTTON, &AppFrame::OnPreviousData, this);

    //Impostazione testi id e channel
    ch_text = new wxStaticText(panel, wxID_ANY, "Channels", wxDefaultPosition, wxDefaultSize, 0, "ch_text");
    id_text = new wxStaticText(panel, wxID_ANY, "ID", wxDefaultPosition, wxDefaultSize, 0, "id_text");
    wxFont tmp_font = ch_text->GetFont();
    tmp_font.SetWeight(wxFONTWEIGHT_BOLD);
    ch_text->SetFont(tmp_font);
    id_text->SetFont(tmp_font);

    //Impostazione tabella per i dati dei messaggi di log
    dataTable = new wxGrid(panel, wxID_ANY, wxDefaultPosition, wxSize(990, -1), wxSUNKEN_BORDER);
    dataTable->CreateGrid(0, num_table_cols);

    wxString cols_titles[] = { "TimeStamp", "ID", "Data", "Channel" };

    int table_width = dataTable->GetSize().GetX();
    int colWidth = table_width / num_table_cols;

    for (int index_col = 0; index_col < num_table_cols; index_col++) {
        dataTable->SetColSize(index_col, colWidth);
        dataTable->SetColLabelValue(index_col, cols_titles[index_col]);
    }

    dataTable->DisableDragColSize();
    dataTable->DisableDragRowSize();
    dataTable->DisableCellEditControl();

    // Creazione del grafico
    plot = new mpWindow(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);


    // Layout principale
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Layout liste
    wxBoxSizer* listSizer = new wxBoxSizer(wxVERTICAL);
    listSizer->Add(ch_text, 0, wxEXPAND | wxALL);
    listSizer->Add(ChannelList, 1, wxEXPAND | wxALL);
    listSizer->Add(id_text, 0, wxEXPAND | wxALL);
    listSizer->Add(IdList, 5, wxEXPAND | wxALL);

    // Layout pulsanti
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(previousDataButton, 1, wxEXPAND | wxALL, 0);
    buttonSizer->Add(nextDataButton, 1, wxEXPAND | wxALL, 0);

    // Layout per grafico, pulsanti e tabella
    wxBoxSizer* button_plot_tableSizer = new wxBoxSizer(wxVERTICAL);
    button_plot_tableSizer->Add(plot, 12, wxEXPAND | wxALL, 0);
    button_plot_tableSizer->Add(buttonSizer, 1, wxEXPAND | wxALL, 0);
    button_plot_tableSizer->Add(dataTable, 3, wxEXPAND | wxALL, 0);
    wxBoxSizer* list_plotSizer = new wxBoxSizer(wxHORIZONTAL);
    list_plotSizer->Add(listSizer, 0, wxEXPAND | wxALL, 10);
    list_plotSizer->Add(button_plot_tableSizer, 1, wxEXPAND | wxALL, 10);

    // Aggiungi il grafico al layout principale
    mainSizer->Add(list_plotSizer, 1, wxEXPAND);

    // Imposta il layout del pannello
    panel->SetSizer(mainSizer);

    SetMenuBar(menuBar);
    CreateStatusBar(4);
    Centre();
}

void AppFrame::OnOpen(wxCommandEvent& event) {
    // Legge i dati
    read_data_from_txt(logs_from_blf);

    //divisione degli oggetti Log per id e per channel
    vector<vector<Log>> id_channel_zero, id_channel_one;
    list_foreach_id(logs_from_blf, id_channel_zero, 0);
    list_foreach_id(logs_from_blf, id_channel_one, 1);

    log_channels.push_back(id_channel_zero);
    log_channels.push_back(id_channel_one);
}

void AppFrame::OnExport(wxCommandEvent& event) {
    write_csv_file(log_channels[selected_channel][selected_id]);
}

void AppFrame::CreatePlot(int channel, int index) {
    plot->DelAllLayers(true); // Pulisce il grafico

    vector<double> data;
    vector<double> time_stmp;

    extract_time_data(log_channels[channel][index], time_stmp, data, visual_limit);

    // Usa la serie selezionata per il grafico
    mpFXYVector* data_layer = new mpFXYVector("", mpALIGN_CENTER);
    data_layer->SetData(time_stmp, data);
    data_layer->SetContinuity(true);
    data_layer->SetPen(wxPen(*wxRED, 2));

    // Asse X e Y
    mpScaleX* xaxis = new mpScaleX("Time", mpALIGN_BORDER_BOTTOM, true);
    mpScaleY* yaxis = new mpScaleY("Data", mpALIGN_BORDER_LEFT, true);

    plot->SetMargins(20, 20, 20, 20);

    plot->AddLayer(xaxis);
    plot->AddLayer(yaxis);
    plot->AddLayer(data_layer);

    plot->EnableMousePanZoom(false);
    plot->Fit();
    plot->Refresh();

}

// 
void AppFrame::UpdateDataTable(vector<Log>& logs) {
    dataTable->ClearGrid();

    if (dataTable->GetNumberRows() > 0)
        dataTable->DeleteRows(0, dataTable->GetNumberRows());

    dataTable->AppendRows(logs.size());

    for (int i = 0; i < logs.size(); i++) {
        for (int j = 0; j < num_table_cols; j++) {
            wxString tmp_cell_text;
            switch (j) {
            case 0:
                tmp_cell_text << logs[i].get_timestamp();
                break;
            case 1:
                tmp_cell_text << logs[i].get_data();
                break;
            case 2:
                tmp_cell_text << logs[i].get_id();
                break;
            case 3:
                tmp_cell_text << logs[i].get_channel();
                break;
            }
            dataTable->SetCellValue(i, j, tmp_cell_text);
            tmp_cell_text.Clear();
        }
    }
}

void AppFrame::OnChannelListSelect(wxCommandEvent& event) {
    int tmp_channel = ChannelList->GetSelection();
    if (tmp_channel != wxNOT_FOUND && !logs_from_blf.empty()) {
        selected_channel = tmp_channel;
        selected_id = 0;

        visual_limit = BASE_VISUAL_LIMIT;

        UpdateDataTable(log_channels[selected_channel][selected_id]);
        CreatePlot(selected_channel, selected_id);

        IdList->Clear();

        vector<vector<Log>> tmp_id_vector = log_channels[selected_channel];

        // Aggiorno la listbox degli id con quelli del channel selezionato
        for (int i = 0; i < tmp_id_vector.size(); i++) {
            wxString tmp_id;
            tmp_id << tmp_id_vector[i][0].get_id();
            IdList->AppendString(tmp_id);
        }
    }
}

void AppFrame::OnIdListSelect(wxCommandEvent& event) {
    int tmp_id = IdList->GetSelection();
    if (tmp_id != wxNOT_FOUND) {
        selected_id = tmp_id;

        visual_limit = BASE_VISUAL_LIMIT;

        UpdateDataTable(log_channels[selected_channel][selected_id]);
        CreatePlot(selected_channel, selected_id);
    }
}

void AppFrame::OnNextData(wxCommandEvent& event) {
    if (logs_from_blf.empty())
        return;
    if (visual_limit > log_channels[selected_channel][selected_id].size())
        return;

    visual_limit += BASE_VISUAL_LIMIT;
    CreatePlot(selected_channel, selected_id);
}

void AppFrame::OnPreviousData(wxCommandEvent& event) {
    if (logs_from_blf.empty())
        return;
    if (visual_limit <= BASE_VISUAL_LIMIT)
        return;

    visual_limit -= BASE_VISUAL_LIMIT;
    CreatePlot(selected_channel, selected_id);
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

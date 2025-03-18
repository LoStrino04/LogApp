#include <wx/wx.h>
#include <wx/grid.h>
#include "lib\\mathplot.h"
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
    wxMenu* export_csv;
    wxMenuItem* export_csv_1;
    wxMenuItem* open_file;
    wxMenuItem* export_csv_2;
    wxMenu* plot_menu;
    wxMenu* export_partial_plot;
    //wxMenu* export_total_plot;
    wxMenuItem* export_part_1;
    wxMenuItem* export_part_2;
    //wxMenuItem* export_tot_1;
    //wxMenuItem* export_tot_2;
    wxMenu* quit_menu;
    wxMenuItem* quit;
    wxMenuItem* close_blf;
    wxButton* first_nextDataButton;
    wxButton* first_previousDataButton;
    wxButton* second_nextDataButton;
    wxButton* second_previousDataButton;
    wxButton* plot_one;
    wxButton* plot_two;
    wxButton* insert_dbc;
    wxButton* delete_dbc;
    wxButton* load_log;
    wxTextCtrl* text_dbc;
    wxTextCtrl* text_log;
    wxStaticText* ch_text;
    wxStaticText* id_text;
    wxStaticText* dbc_text;
    vector<LogMsg> logs_from_blf;
    vector<vector<LogSignal>> log_channels;

    // variabili per visualizzazione grafici
    int first_selected_channel = 0, first_selected_id = 0, first_visual_limit = BASE_VISUAL_LIMIT;
    int second_selected_channel = 0, second_selected_id = 0, second_visual_limit = BASE_VISUAL_LIMIT;
    int tmp_selected_plot = 0;

    void CreatePlot(int channel, int index, int visual_limit, mpWindow* in_plot);
    void ExportPlotPNG(mpWindow* in_plot, int channel, int index);
    void OnOpen(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);
    void OnCloseBLF(wxCommandEvent& event);
    void OnChannelListSelect(wxCommandEvent& event);
    void OnIdListSelect(wxCommandEvent& event);
    void OnFirstNextData(wxCommandEvent& event);
    void OnFirstPreviousData(wxCommandEvent& event);
    void OnSecondNextData(wxCommandEvent& event);
    void OnSecondPreviousData(wxCommandEvent& event);
    void OnExportUpperCSV(wxCommandEvent& event);
    void OnExportLowerCSV(wxCommandEvent& event);
    void OnPlotOne(wxCommandEvent& event);
    void OnPlotTwo(wxCommandEvent& event);
    void OnInsertDBC(wxCommandEvent& event);
    void OnDeleteDBC(wxCommandEvent& event);
    void OnExportUpperPlot(wxCommandEvent& event);
    void OnExportLowerPlot(wxCommandEvent& event);
    //void OnExportTotalUpperPlot(wxCommandEvent& event);
    //void OnExportTotalLowerPlot(wxCommandEvent& event);
};

AppFrame::AppFrame(const wxString& titolo, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, wxID_ANY, titolo, pos, size) {

    wxPanel* panel = new wxPanel(this, wxID_ANY);

    //creazione barra dei menu (le righe commentate sono per l'esportazione del grafico completo non ancora implementata)
    menuBar = new wxMenuBar();
    file_menu = new wxMenu();
    export_csv = new wxMenu();
    plot_menu = new wxMenu();
    export_partial_plot = new wxMenu();
    //export_total_plot = new wxMenu();
    quit_menu = new wxMenu();
    export_part_1 = new wxMenuItem(export_partial_plot, 0, "Upper Plot");
    export_part_2 = new wxMenuItem(export_partial_plot, 1, "Lower Plot");
    //export_tot_1 = new wxMenuItem(export_total_plot, 2, "Upper Plot");
    //export_tot_2 = new wxMenuItem(export_total_plot, 3, "Lower Plot");
    export_csv_1 = new wxMenuItem(export_csv, 4, "Export CSV Upper Signal");
    export_csv_2 = new wxMenuItem(export_csv, 5, "Export CSV Lower Signal");
    quit = new wxMenuItem(quit_menu, 6, "Close App");
    close_blf = new wxMenuItem(file_menu, 7, "Close BLF");

    export_csv->Append(export_csv_1);
    export_csv->Append(export_csv_2);
    export_csv->Bind(wxEVT_MENU, &AppFrame::OnExportUpperCSV, this, export_csv_1->GetId());
    export_csv->Bind(wxEVT_MENU, &AppFrame::OnExportLowerCSV, this, export_csv_2->GetId());
    file_menu->AppendSubMenu(export_csv, "Export CSV", wxEmptyString);
    file_menu->Append(close_blf);
    file_menu->Bind(wxEVT_MENU, &AppFrame::OnCloseBLF, this, close_blf->GetId());
    //export_total_plot->Append(export_tot_1);
    //export_total_plot->Append(export_tot_2);
    //export_total_plot->Bind(wxEVT_MENU, &AppFrame::OnExportTotalUpperPlot, this, export_tot_1->GetId());
    //export_total_plot->Bind(wxEVT_MENU, &AppFrame::OnExportTotalLowerPlot, this, export_tot_2->GetId());
    export_partial_plot->Append(export_part_1);
    export_partial_plot->Append(export_part_2);
    export_partial_plot->Bind(wxEVT_MENU, &AppFrame::OnExportUpperPlot, this, export_part_1->GetId());
    export_partial_plot->Bind(wxEVT_MENU, &AppFrame::OnExportLowerPlot, this, export_part_2->GetId());
    plot_menu->AppendSubMenu(export_partial_plot, "Export Partial", wxEmptyString);
    quit_menu->Append(quit);
    quit_menu->Bind(wxEVT_MENU, &AppFrame::OnClose, this, quit->GetId());
    menuBar->Append(file_menu, "File");
    menuBar->Append(plot_menu, "Plot");
    menuBar->Append(quit_menu, "Quit");


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
    text_dbc->SetHint("Insert DBC Path Here");

    // impostazione pulsante e casella di testo per inserimento file blf
    load_log = new wxButton(panel, wxID_ANY, "Load LOG", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "load_log");
    load_log->Bind(wxEVT_BUTTON, &AppFrame::OnOpen, this);
    text_log = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "text_log");
    text_log->SetHint("Insert File BLF Path Here");

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

    // Layout pulsanti per selezionare quale grafico impostare
    wxBoxSizer* selection_buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    selection_buttonSizer->Add(plot_one, 1, wxEXPAND | wxALL, 0);
    selection_buttonSizer->Add(plot_two, 1, wxEXPAND | wxALL, 0);
    // Layput pulsanti per spostarsi nel  primo grafico
    wxBoxSizer* first_buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    first_buttonSizer->Add(first_previousDataButton, 1, wxEXPAND | wxALL, 0);
    first_buttonSizer->Add(first_nextDataButton, 1, wxEXPAND | wxALL, 0);
    // Layout pulsanti per spostarsi nel secondo grafico
    wxBoxSizer* second_buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    second_buttonSizer->Add(second_previousDataButton, 1, wxEXPAND | wxALL, 0);
    second_buttonSizer->Add(second_nextDataButton, 1, wxEXPAND | wxALL, 0);

    // Layout pulsanti e textbox per inserimento dbc e blf
    wxBoxSizer* dbc_sizer = new wxBoxSizer(wxHORIZONTAL);
    dbc_sizer->Add(insert_dbc, 1, wxEXPAND | wxALL, 0);
    dbc_sizer->Add(text_dbc, 5, wxEXPAND | wxALL, 0);
    wxBoxSizer* log_sizer = new wxBoxSizer(wxHORIZONTAL);
    log_sizer->Add(load_log, 1, wxEXPAND | wxALL, 0);
    log_sizer->Add(text_log, 5, wxEXPAND | wxALL, 0);

    // Layout per la parte di interfaccia dei grafici
    wxBoxSizer* button_plotSizer = new wxBoxSizer(wxVERTICAL);
    button_plotSizer->Add(log_sizer, 0, wxEXPAND | wxALL, 0);
    button_plotSizer->Add(dbc_sizer, 0, wxEXPAND | wxALL, 0);
    button_plotSizer->Add(selection_buttonSizer, 1, wxEXPAND | wxALL, 0);
    button_plotSizer->Add(first_plot, 12, wxEXPAND | wxALL, 0);
    button_plotSizer->Add(first_buttonSizer, 1, wxEXPAND | wxALL, 0);
    button_plotSizer->Add(second_plot, 12, wxEXPAND | wxALL, 0);
    button_plotSizer->Add(second_buttonSizer, 1, wxEXPAND | wxALL, 0);
    wxBoxSizer* list_plotSizer = new wxBoxSizer(wxHORIZONTAL);
    list_plotSizer->Add(listSizer, 1, wxEXPAND | wxALL, 10);
    list_plotSizer->Add(button_plotSizer, 5, wxEXPAND | wxALL, 10);

    // Aggiunta layout pagina al layout principale
    mainSizer->Add(list_plotSizer, 1, wxEXPAND);

    // Impostazione il layout del pannello
    panel->SetSizer(mainSizer);

    write_dbc_list(DbcList);
    SetMenuBar(menuBar);
    CreateStatusBar(2);
    Centre();
}

// Evento di apertura del file blf
void AppFrame::OnOpen(wxCommandEvent& event) {

    if (!logs_from_blf.empty()) {
        wxMessageBox("Attenzione! E' già stato caricato un file BLF", "Warning", wxOK | wxICON_WARNING);
        return;
    }
    if (text_log->IsEmpty()) {
        wxMessageBox("Attenzione! Nessun file BLF selezionato", "Warning", wxOK | wxICON_WARNING);
        return;
    }

    ofstream blf_name(blf_name_file);
    blf_name.clear();
    blf_name << text_log->GetValue();
    blf_name.close();

    // Legge i dati dal file blf
    read_data_from_txt(logs_from_blf);
    //divisione degli oggetti Log per id e per channel
    vector<LogSignal> id_channel_zero, id_channel_one;
    list_foreach_id(logs_from_blf, id_channel_zero, 0);
    list_foreach_id(logs_from_blf, id_channel_one, 1);
    assign_name_to_id(id_channel_zero);
    assign_name_to_id(id_channel_one);

    log_channels.push_back(id_channel_zero);
    log_channels.push_back(id_channel_one);

    wxMessageBox("Caricamento completato!", "Informazione", wxOK | wxICON_INFORMATION);
}

// Chiusura applicazione
void AppFrame::OnCloseBLF(wxCommandEvent& event) {
    log_channels.clear();
    logs_from_blf.clear();
    first_plot->DelAllLayers(true);
    second_plot->DelAllLayers(true);
    IdList->Clear();
    ofstream id_names_file(id_name_file);
    id_names_file.clear();
    id_names_file.close();
}

void AppFrame::OnClose(wxCommandEvent& event) {
    ofstream id_names_file(id_name_file);
    id_names_file.clear();
    id_names_file.close();
    Destroy();
}

// funzione di creazione del grafico (passato come parametro) di un segnale selezionato
void AppFrame::CreatePlot(int channel, int index, int visual_limit, mpWindow* in_plot) {

    in_plot->DelAllLayers(true); // Pulisce il grafico

    vector<double> data;
    vector<double> time_stmp;

    extract_time_data(log_channels[channel][index].get_logs(), time_stmp, data, visual_limit);

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

void AppFrame::ExportPlotPNG(mpWindow* in_plot, int channel, int index) {
    if (in_plot->CountLayers() == 0) {
        wxMessageBox("Attenzione! Grafico superiore vuoto", "Warning", wxOK | wxICON_WARNING);
        return;
    }
    wxInitAllImageHandlers();
    // Creare un wxBitmap della dimensione della finestra del grafico
    wxSize size = in_plot->GetClientSize();
    wxBitmap bmp(size.x, size.y);

    // Creare un wxClientDC per catturare il contenuto della finestra
    wxClientDC dc(in_plot);
    wxMemoryDC memDC;
    memDC.SelectObject(bmp);
    memDC.Blit(0, 0, size.x, size.y, &dc, 0, 0);            // Copia il contenuto del DC
    memDC.SelectObject(wxNullBitmap);                       // Rilascia il wxBitmap

    // Convertire il bitmap in immagine e salvarlo come PNG
    wxImage img = bmp.ConvertToImage();
    wxString filename = "partial_" + log_channels[channel][index].get_name() + ".png";
    if (img.SaveFile("PNG\\ " + filename, wxBITMAP_TYPE_PNG))
        wxMessageBox("Grafico salvato come " + filename, "Salvataggio", wxOK | wxICON_INFORMATION);
    else
        wxMessageBox("Errore nel salvataggio!", "Errore", wxOK | wxICON_ERROR);
}

// esportazione in csv del segnale presente nel grafico superiore
void AppFrame::OnExportUpperCSV(wxCommandEvent& event) {
    if (first_plot->CountLayers() == 0) {
        wxMessageBox("Attenzione! Nessun grafico impostato", "Warning", wxOK | wxICON_WARNING);
        return;
    }
    if (write_csv_file(log_channels[first_selected_channel][first_selected_id])) {
        string file_name = log_channels[first_selected_channel][first_selected_id].get_name();
        wxMessageBox("Il segnale " + file_name + " salvato in csv come csv_" + file_name, "Salvataggio", wxOK | wxICON_INFORMATION);
    }
}

// esportazione in csv del grafico presente nel grafico inferiore
void AppFrame::OnExportLowerCSV(wxCommandEvent& event) {
    if (second_plot->CountLayers() == 0) {
        wxMessageBox("Attenzione! Nessun grafico impostato", "Warning", wxOK | wxICON_WARNING);
        return;
    }
    if (write_csv_file(log_channels[second_selected_channel][second_selected_id])) {
        string file_name = log_channels[second_selected_channel][second_selected_id].get_name();
        wxMessageBox("Il segnale " + file_name + " salvato in csv come csv_" + file_name, "Salvataggio", wxOK | wxICON_INFORMATION);
    }
}

void AppFrame::OnExportUpperPlot(wxCommandEvent& event) {
    ExportPlotPNG(first_plot, first_selected_channel, first_selected_id);
}

void AppFrame::OnExportLowerPlot(wxCommandEvent& event) {
    ExportPlotPNG(second_plot, second_selected_channel, second_selected_id);
}

/********** Esportazione grafico completo (non ancora implementata)*********
void AppFrame::OnExportTotalUpperPlot(wxCommandEvent& event) {
    write_csv_file(log_channels[first_selected_channel][first_selected_id]);
    string py_file = "export_total_plot.py";
    run_python_file(py_file);
}

void AppFrame::OnExportTotalLowerPlot(wxCommandEvent& event) {
    write_csv_file(log_channels[second_selected_channel][second_selected_id]);
    string py_file = "export_total_plot.py";
    run_python_file(py_file);
}
*****************************************************************************/

// funzione per selezionare il canale di cui visualizzare gli id
void AppFrame::OnChannelListSelect(wxCommandEvent& event) {
    int tmp_channel = ChannelList->GetSelection();
    if (tmp_channel != wxNOT_FOUND && !logs_from_blf.empty()) {

        // selezione grafico da impostare
        if (tmp_selected_plot == 0) {
            first_selected_channel = tmp_channel;
            first_selected_id = 0;
        }
        else {
            second_selected_channel = tmp_channel;
            second_selected_id = 0;
        }

        IdList->Clear();
        vector<LogSignal> tmp_id_vector = log_channels[tmp_channel];

        // Aggiorno la listbox degli id con quelli del channel selezionato
        for (int i = 0; i < tmp_id_vector.size(); i++) {
            wxString tmp_id;
            if (tmp_id_vector[i].get_name() == "") {
                stringstream stream;
                stream << hex << tmp_id_vector[i].first().get_id();
                string result(stream.str());
                tmp_id << "ID: " << result;
            }
            else
                tmp_id << tmp_id_vector[i].get_name();

            IdList->AppendString(tmp_id);
        }
    }
}

// funzione per visualizzare l'id selezionato
void AppFrame::OnIdListSelect(wxCommandEvent& event) {
    int tmp_id = IdList->GetSelection();
    if (tmp_id != wxNOT_FOUND) {

        //selezione grafico da impostare
        if (tmp_selected_plot == 0) {
            first_selected_id = tmp_id;
            first_visual_limit = BASE_VISUAL_LIMIT;
            CreatePlot(first_selected_channel, first_selected_id, first_visual_limit, first_plot);
            SetStatusText("Upper Plot: " + log_channels[first_selected_channel][first_selected_id].get_name(), 0);
        }
        else {
            second_selected_id = tmp_id;
            second_visual_limit = BASE_VISUAL_LIMIT;
            CreatePlot(second_selected_channel, second_selected_id, second_visual_limit, second_plot);
            SetStatusText("Lower Plot: " + log_channels[second_selected_channel][second_selected_id].get_name(), 1);
        }
    }
}

// funzione inserimento dbc nel file di testo dbc_names e nella listbox dedicato alle dbc
void AppFrame::OnInsertDBC(wxCommandEvent& event) {
    if (text_dbc->IsEmpty()) {
        wxMessageBox("Attenzione! Inserire prima il percorso della DBC", "Warning", wxOK | wxICON_WARNING);
        return;
    }

    wxString dbc_path = text_dbc->GetValue();
    wxString wx_dbc_name;

    // controllo se la dbc è già presente nel listbox
    for (int i = 0; i < DbcList->GetCount(); i++) {
        wxString tmp_str = DbcList->GetString(i);

        if (tmp_str == getFileName(string(dbc_path)))
            return;
    }

    DbcList->AppendString(dbc_path);
    write_dbc_path(text_dbc);

    wx_dbc_name << getFileName(string(dbc_path));

    DbcList->Delete(DbcList->GetCount() - 1);
    DbcList->AppendString(wx_dbc_name);
    text_dbc->Clear();
}

// cancellazione dbc
void AppFrame::OnDeleteDBC(wxCommandEvent& event) {
    int tmp_dbc_ind = DbcList->GetSelection();

    if (tmp_dbc_ind != wxNOT_FOUND) {
        wxString tmp_dbc_name = DbcList->GetString(tmp_dbc_ind);
        delete_dbc_path(tmp_dbc_name);
        DbcList->Delete(tmp_dbc_ind);
    }
}

// selezione grafico superiore
void AppFrame::OnPlotOne(wxCommandEvent& event) {
    tmp_selected_plot = 0;
    plot_one->SetBackgroundColour(*wxLIGHT_GREY);
    plot_two->SetBackgroundColour(*wxWHITE);
}
// selezione grafico inferiore
void AppFrame::OnPlotTwo(wxCommandEvent& event) {
    tmp_selected_plot = 1;
    plot_one->SetBackgroundColour(*wxWHITE);
    plot_two->SetBackgroundColour(*wxLIGHT_GREY);
}

// funzioni per spostarsi nel grafico superiore
void AppFrame::OnFirstNextData(wxCommandEvent& event) {
    if (logs_from_blf.empty() || first_plot->CountLayers() == 0)
        return;
    if (first_visual_limit > log_channels[first_selected_channel][first_selected_id].get_logs().size())
        return;

    first_visual_limit += BASE_VISUAL_LIMIT;
    CreatePlot(first_selected_channel, first_selected_id, first_visual_limit, first_plot);
}

void AppFrame::OnFirstPreviousData(wxCommandEvent& event) {
    if (logs_from_blf.empty() || first_plot->CountLayers() == 0)
        return;
    if (first_visual_limit <= BASE_VISUAL_LIMIT)
        return;

    first_visual_limit -= BASE_VISUAL_LIMIT;
    CreatePlot(first_selected_channel, first_selected_id, first_visual_limit, first_plot);
}

// funzioni per spostarsi nel grafico inferiore
void AppFrame::OnSecondNextData(wxCommandEvent& event) {
    if (logs_from_blf.empty() || second_plot->CountLayers() == 0)
        return;
    if (second_visual_limit > log_channels[second_selected_channel][second_selected_id].get_logs().size())
        return;

    second_visual_limit += BASE_VISUAL_LIMIT;
    CreatePlot(second_selected_channel, second_selected_id, second_visual_limit, second_plot);
}

void AppFrame::OnSecondPreviousData(wxCommandEvent& event) {
    if (logs_from_blf.empty() || second_plot->CountLayers() == 0)
        return;
    if (second_visual_limit <= BASE_VISUAL_LIMIT)
        return;

    second_visual_limit -= BASE_VISUAL_LIMIT;
    CreatePlot(second_selected_channel, second_selected_id, second_visual_limit, second_plot);
}

class LogApp : public wxApp {
public:
    virtual bool OnInit();
};

bool LogApp::OnInit() {
    AppFrame* frame = new AppFrame("LogApp", wxPoint(50, 50), wxSize(1280, 720));
    wxIcon icon;
    icon.LoadFile("icons\\icon.ico", wxBITMAP_TYPE_ICO);
    frame->SetIcon(icon);
    frame->Show(true);
    return true;
}

wxIMPLEMENT_APP(LogApp);

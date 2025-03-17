#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <string>
#include "log.h"

#define BASE_VISUAL_LIMIT 300		//lmite di dati visualizzabili in una volta nel grafico
using namespace std;

// funzione che trasmorma un oggetto LogMsg in una stringa in formato csv
string log_to_string(LogMsg in_log_msg) {
	string out_string = "";

	return out_string + to_string(in_log_msg.get_timestamp()) + "," + to_string(in_log_msg.get_id()) + "," + to_string(in_log_msg.get_data()) + "," + to_string(in_log_msg.get_channel());
}

//funzione per eseguire script python per leggere i dati da file blf
void run_python_file(string& file_name) {
	ifstream open_file("TXT\\open_info.txt");
	string command;

	open_file >> command;
	command += (" " + file_name);

	open_file.close();

	system(command.c_str());
}

// funzione per estrarre il nome di un file dato il suo percorso
string getFileName(const string filePath) {
	size_t lastSlash = filePath.find_last_of("\\/");			// Trova l'ultimo separatore
	if (lastSlash != std::string::npos) {
		return filePath.substr(lastSlash + 1);					// Estrae il nome del file
	}
	return filePath;											// Se non ci sono separatori, restituisce l'intera stringa
}

//ricerca un messaggio di log in base all'ID (utilizzata in list_foreach_id) e ne restituisce l'indice in list_for_id
int search_msg_by_id(vector<LogSignal>& in_list, int id_to_search) {
	int index_id = 0;

	for (int i = 0; i < in_list.size(); i++) {
		if (in_list[i].first().get_id() == id_to_search)
			return index_id;

		index_id++;
	}
	return -1;
}

//legge i dati di ogni messaggio contenuto nel file txt in formato csv e li memorizza in un vettore di oggetti Log
void read_data_from_txt(vector<LogMsg>& L_log)
{

	string filename = "read_blf.py";
	run_python_file(filename);

	//Lettura file txt con dati del file blf
	ifstream log_file("TXT\\log_datas.txt");
	if (log_file.is_open()) {
		string str_timestamp, str_datas, str_id, str_channel, str_name;
		while (getline(log_file, str_timestamp, ',')) {
			getline(log_file, str_datas, ',');
			getline(log_file, str_id, ',');
			getline(log_file, str_channel, ',');
			getline(log_file, str_name);

			istringstream tk_tm(str_timestamp);
			double tmp_timestamp;
			tk_tm >> tmp_timestamp;

			istringstream tk_dt(str_datas);
			double tmp_datas;
			tk_dt >> tmp_datas;

			istringstream tk_id(str_id);
			int tmp_id;
			tk_id >> tmp_id;

			istringstream tk_ch(str_channel);
			int tmp_ch;
			tk_ch >> tmp_ch;

			//Creazione di un array di elementi della classe Log
			LogMsg tmp_log(tmp_timestamp, tmp_datas, tmp_id, tmp_ch, str_name);
			L_log.push_back(tmp_log);

		}
	}

	log_file.close();

	//cancello il contenuto del file txt usato per passare i dati
	ofstream file("TXT\\log_datas.txt", ios::trunc);

}


//crea un vettore di vettori di log (di un canale dato) divisi per ID
void list_foreach_id(vector<LogMsg>& log_list, vector<LogSignal>& list_for_id, int in_channel) {
	for (int i = 0; i < log_list.size(); i++) {
		LogMsg tmp_log = log_list[i];

		if (tmp_log.get_channel() != in_channel)
			continue;

		int id_index = search_msg_by_id(list_for_id, tmp_log.get_id());

		if (id_index >= 0) {
			list_for_id[id_index].add_log_msg(tmp_log);
		}
		else {
			vector<LogMsg> new_id_vector;
			new_id_vector.push_back(tmp_log);
			LogSignal new_signal;
			new_signal.set_logs(new_id_vector);
			list_for_id.push_back(new_signal);
		}
	}

	return;
}

//estraggo da un vettore di log i timestamp e i data, visual limit imposta il numero di dati estratti
void extract_time_data(vector<LogMsg> in_logs, vector<double>& in_time, vector<double>& in_data, int visual_limit) {

	for (int i = visual_limit - BASE_VISUAL_LIMIT; i < in_logs.size(); i++) {

		if (i > visual_limit)
			break;

		LogMsg tmp_log = in_logs[i];

		in_time.push_back(tmp_log.get_timestamp());
		in_data.push_back(tmp_log.get_data());
	}
}

// Scrittura file in formato csv (il file txt è presente nella cartella dell'eseguibile)
bool write_csv_file(LogSignal to_export_logs) {
	string tmp_log_name = to_export_logs.get_name();
	ofstream export_csv_file("CSV\\csv_" + tmp_log_name + ".txt");

	for (int i = 0; i < to_export_logs.get_logs().size(); i++) {
		export_csv_file << log_to_string(to_export_logs.get_logs()[i]) << endl;
	}

	export_csv_file.close();

	return true;
}

// funzione per scrivere il path della dbc inserita dal textbox nel file dbc_names.txt
void write_dbc_path(wxTextCtrl* dbc_name) {
	ofstream dbc_names_file;
	dbc_names_file.open("TXT\\dbc_names.txt", ios_base::app);

	wxString tmp_dbc_path = dbc_name->GetValue();

	dbc_names_file << tmp_dbc_path << endl;
	dbc_names_file.close();
}

// funzione per cancellare il path di una dbc dal file dbc_names.txt
void delete_dbc_path(wxString dbc_name) {
	ifstream dbc_names_file("TXT\\dbc_names.txt");
	string tmp_dbc;
	vector<string> tmp_dbc_list;

	while (dbc_names_file >> tmp_dbc) {								//copio i path delle dbc presenti nel file in un vettore temporaneo
		if (dbc_name.ToStdString() != getFileName(tmp_dbc)) {		//tranne il path di quella da cancellare
			tmp_dbc_list.push_back(tmp_dbc);
		}
	}
	dbc_names_file.close();

	ofstream dbc_names_out_file("TXT\\dbc_names.txt");					//riscrivo il file con i path rimanenti

	for (int i = 0; i < tmp_dbc_list.size(); i++) {
		dbc_names_out_file << tmp_dbc_list[i] << endl;
	}
}

// funzione per scrivere nel listbox delle dbc il nome di ciascuna dbc presente nel file dbc_names.txt
void write_dbc_list(wxListBox* dbc_list) {
	ifstream dbc_names_file("TXT\\dbc_names.txt");
	string str_path;

	while (dbc_names_file >> str_path) {
		string str_name = getFileName(str_path);
		wxString wx_name;
		wx_name << str_name;
		dbc_list->AppendString(wx_name);
	}

	dbc_names_file.close();
}

// funzione per assegnare il nome a un messaggio di log corrispondente al suo id
void assign_name_to_id(vector<LogSignal>& log_signals) {
	ifstream id_names_file("TXT\\id_names.txt");
	string str_id, str_name;

	while (getline(id_names_file, str_id, ',')) {
		getline(id_names_file, str_name);

		istringstream tk_id(str_id);
		int tmp_id;
		tk_id >> tmp_id;
		for (int i = 0; i < log_signals.size(); i++) {
			if (log_signals[i].first().get_id() == tmp_id) {
				log_signals[i].set_name(str_name);
			}
		}
	}

	id_names_file.clear();
	id_names_file.close();
}

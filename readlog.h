#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <cmath>
#include <vector>
#include <list>

#define BASE_VISUAL_LIMIT 300		//lmite di dati visualizzabili in una volta nel grafico

using namespace std;

//classe Log che contiene i dati di ogni messaggio del file blf
class Log {
	private:
		double timestamp;
		double data;
		int id;
		int channel;
		string name;
	public:
		Log(double in_time, double in_data, int in_id, int in_channel, string in_name) {
			timestamp = in_time;
			data = in_data;
			id = in_id;
			channel = in_channel;
			name = in_name;
		}
		double get_timestamp() const { return timestamp; }
		double get_data() const { return data; }
		int get_id() const { return id; }
		int get_channel() const { return channel; }
		string get_name() const { return name; }
};

string log_to_string(Log in_log_msg) {
	string out_string = "";

	return out_string + to_string(in_log_msg.get_timestamp()) + ", " + to_string(in_log_msg.get_id()) + ", " + to_string(in_log_msg.get_data()) + ", " + to_string(in_log_msg.get_channel());
}


//funzione per eseguire script python per leggere i dati da file blf
void run_python_file(string& file_name) {
	string command = "py ";

	command += file_name;
	system(command.c_str());
}

//legge i dati di ogni messaggio contenuto nel file txt in formato csv e li memorizza in un vettore di oggetti Log
void read_data_from_txt(vector<Log>& L_log) 
{

	string filename = "read_blf.py";
	run_python_file(filename);

	//Lettura file txt con dati del file blf
	ifstream log_file("log_datas.txt");
	if (log_file.is_open()) {
		string str_timestamp, str_datas, str_id, str_channel, str_name;
		while (getline(log_file, str_name, ',')) {
			getline(log_file, str_timestamp, ',');
			getline(log_file, str_datas, ',');
			getline(log_file, str_id, ',');
			getline(log_file, str_channel);

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
			Log tmp_log(tmp_timestamp, tmp_datas, tmp_id, tmp_ch, str_name);
			L_log.push_back(tmp_log);

		}
	}

	log_file.close();

	//cancello il contenuto del file txt usato per passare i dati
	ofstream file("log_datas.txt", ios::trunc);

}


//ricerca un messaggio di log in base all'ID (utilizzata in list_foreach_id) e ne restituisce l'indice in list_for_id
int search_msg_by_id(vector<vector<Log>>& in_list, int id_to_search) {
	int index_id = 0;

	for (int i = 0; i < in_list.size(); i++) {
		if (in_list[i][0].get_id() == id_to_search)
			return index_id;

		index_id++;
	}
	return -1;
}

//crea un vettore di vettori di log (di un canale dato) divisi per ID
void list_foreach_id(vector<Log>& log_list, vector<vector<Log>>& list_for_id, int in_channel) {
	for (int i = 0; i < log_list.size(); i++) {
		Log tmp_log = log_list[i];

		if (tmp_log.get_channel() != in_channel)
			continue;

		int id_index = search_msg_by_id(list_for_id, tmp_log.get_id());

		if (id_index >= 0) {
			list_for_id[id_index].push_back(tmp_log);
		}
		else {
			vector<Log> new_id_vector;
			new_id_vector.push_back(tmp_log);
			list_for_id.push_back(new_id_vector);
		}
	}

	return;
}

//estraggo da un vettore di log i timestamp e i data, visual limit imposta il numero di dati estratti
void extract_time_data(vector<Log> in_logs, vector<double>& in_time, vector<double>& in_data, int visual_limit) {
	
	for (int i = visual_limit - BASE_VISUAL_LIMIT; i < in_logs.size(); i++) {

		if (i > visual_limit)
			break;

		Log tmp_log = in_logs[i];

		in_time.push_back(tmp_log.get_timestamp());
		in_data.push_back(tmp_log.get_data());

	}
	return;
}

// Scrittura file in formato csv
void write_csv_file(vector<Log> to_export_logs) {
	ofstream export_csv_file("csv_log_export.txt");

	for (int i = 0; i < to_export_logs.size(); i++) {
		export_csv_file << log_to_string(to_export_logs[i]) << endl;
	}

	export_csv_file.close();
}

void write_dbc_path(wxListBox* dbc_list) {
	ofstream dbc_names_file("dbc_names.txt");
	dbc_names_file.clear();

	for (int i = 0; i < dbc_list->GetCount(); i++) {
		wxString tmp_dbc_path = dbc_list->GetString(i);
		
		dbc_names_file << tmp_dbc_path << endl;
	}
	
	dbc_names_file.close();
}

void write_dbc_list(wxListBox* dbc_list) {
	ifstream dbc_names_file("dbc_names.txt");
	string str_path;

	while (dbc_names_file >> str_path) {
		dbc_list->AppendString(str_path);
	}

	dbc_names_file.close();
}